#include <spdlog/spdlog.h>
#include <AquaEngine/Timer.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Utils.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <mono/metadata/threads.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

// For adding internal calls to mono. Binding C++ to C#
#include <AquaEngine/Glue.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

namespace fs = std::filesystem;

const char* AssembliesPath = "app://Assets/Mono/";
const char* AppDomainName = "AquaEngineAppDomain";

string ScriptEngine::s_CoreDLLPath = "";
bool ScriptEngine::s_AwaitingReload = false;
bool ScriptEngine::s_DebuggingEnabled = false;
MonoDomain* ScriptEngine::s_AppDomain = nullptr;
Assembly* ScriptEngine::s_CoreAssembly = nullptr;
MonoDomain* ScriptEngine::s_RootDomain = nullptr;
vector<Assembly*> ScriptEngine::s_Assemblies = {};
vector<function<void()>> ScriptEngine::s_ReloadCallbacks = {};
vector<function<void()>> ScriptEngine::s_PreReloadCallbacks = {};

vector<ScriptEngine::AssemblyPath> ScriptEngine::s_AssemblyPaths = {};

const char* MonoDebugLogPath = "data://MonoDebugger.log";

void JITParseOptions(unsigned int debugPort)
{
	Application* app = Application::Current();
	vector<const char*> options = {};
	bool waitForDebugger = app->HasArg("WaitForDebug");

	options.push_back("--soft-breakpoints");

	string debugging = "--debugger-agent=transport=dt_socket,server=y,";
	debugging += "suspend=" + string(waitForDebugger ? "y" : "n") + ",";
	debugging += "address=0.0.0.0:" + to_string(debugPort);
	debugging += ",loglevel=" + app->GetArg("DebugLogLevel", "1");
	debugging += ",logfile=" + VFS::GetAbsolutePath(MonoDebugLogPath);

	options.push_back(debugging.c_str());

	if (waitForDebugger)
		spdlog::info("Waiting for debugger to attach before proceeding...");

	mono_jit_parse_options((int)options.size(), (char**)options.data());
}

bool TryLoadDebugSymbols(MonoImage* image, fs::path& path)
{
	if (!VFS::Exists(path.string()))
		return false;

	auto pdbContents = VFS::Read(path.string());
	mono_debug_open_image_from_memory(image, (const mono_byte*)pdbContents.data(), (int)pdbContents.size());
	spdlog::trace("Added debug symbols found at '{}'", path.string().c_str());
	return true;
}

void ScriptEngine::Init(std::string& coreDllPath, bool allowDebugging)
{
	s_CoreDLLPath = coreDllPath;
	if (s_RootDomain)
		return; // Already initialised

	s_DebuggingEnabled = allowDebugging;

	string assembliesPath = VFS::GetAbsolutePath(AssembliesPath);
	if (!VFS::Exists(assembliesPath))
	{
		spdlog::error("Mono assemblies path is invalid, disabling scripting");
		return;
	}
	mono_set_assemblies_path(assembliesPath.c_str());

	// Setup debugging session
	if (allowDebugging)
	{
		unsigned int debugPort = 5555;
		try { debugPort = std::stoul(Application::Current()->GetArg("DebugPort", "5555")); }
		catch(std::exception&) { spdlog::warn("'DebugPort' argument was not a valid number, defaulting to '5555'"); }

		JITParseOptions(debugPort);
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);

		spdlog::debug("C# debugging is enabled on port {}", debugPort);
	}

	s_RootDomain = mono_jit_init("AquaEngineRuntime");
	if (!s_RootDomain)
	{
		spdlog::critical("Failed to create mono domain");
		return;
	}

	if (allowDebugging)
		mono_debug_domain_create(s_RootDomain);

	mono_thread_set_main(mono_thread_current());

	s_AppDomain = mono_domain_create_appdomain((char*)AppDomainName, nullptr);
	mono_domain_set(s_AppDomain, true);

	LoadCoreAssembly();

	// Add AquaScriptCore internal methods
	AddInternalCalls(_InternalMethods);
}

void ScriptEngine::Destroy()
{
	if (!s_RootDomain)
		return; // Already destroyed

	mono_jit_cleanup(s_RootDomain);

	s_AppDomain = nullptr;
	s_RootDomain = nullptr;
}

Assembly* ScriptEngine::GetCoreAssembly() { return s_CoreAssembly; }
vector<Assembly*>& ScriptEngine::GetAssemblies() { return s_Assemblies; }
Assembly* ScriptEngine::LoadAssembly(string path, bool shouldWatch) { return LoadAssembly(path, false, shouldWatch); }

Assembly* ScriptEngine::LoadAssembly(string& path, bool isCoreAssembly, bool shouldWatch)
{
	if (path.empty())
		return nullptr;
	if (!IO::VFS::Exists(path))
	{
		spdlog::warn("Failed to load '{}' - could not be found", path);
		return nullptr;
	}

	spdlog::debug("Loading C# script from '{}'", path);
	auto data = IO::VFS::Read(path);
	if (data.empty())
	{
		spdlog::warn("Failed to load '{}' - failed to read", path);
		return nullptr;
	}

	if (/* !isCoreAssembly && */ shouldWatch)
	{
		s_AssemblyPaths.push_back({ path, shouldWatch });

		// Watch files and reload if any changes occur
		if (shouldWatch)
			IO::VFS::GetMapping(path)->Watch(path, ScriptEngine::OnAssemblyFileChanged);
	}

	MonoImageOpenStatus status;
	MonoImage* image = mono_image_open_from_data_full(
		reinterpret_cast<char*>(data.data()),
		(uint32_t)data.size(),
		1, 	// Bool. Copy data to internal mono buffer
		&status,
		0	// Bool. Load in reflection mode
	);

	if (status != MONO_IMAGE_OK)
	{
		spdlog::error("Failed to load '{}' - '{}'", path, mono_image_strerror(status));
		return nullptr;
	}

	// Load debugging info
	if (DebuggingEnabled())
	{
		// Try to load debug symbols for assembly
		std::filesystem::path pdbPath(path);
		
#if !defined(AQUA_PLATFORM_WINDOWS) // .pdb files generated by Visual Studio don't work with mono
		// Check for .pdb (must be compiled as 'portable')
		pdbPath.replace_extension(".pdb");
		TryLoadDebugSymbols(image, pdbPath);
#endif

		// Try mono debug symbols if present
		pdbPath.replace_extension(".dll.mdb");
		TryLoadDebugSymbols(image, pdbPath);
	}

	MonoAssembly* assembly = mono_assembly_load_from_full(
		image,
		path.c_str(), // Friendly name, used for warnings & errors
		&status,
		0	// Bool. Load in reflection mode
	);
	mono_image_close(image);

	Assembly* instance = new Assembly(assembly, isCoreAssembly);
	s_Assemblies.push_back(instance);
	return instance;
}

vector<const char*> CoreAssemblyDependencies =
{
	// "app://Newtonsoft.Json.dll"
};

void ScriptEngine::LoadCoreAssembly()
{
	for(size_t i = 0; i < CoreAssemblyDependencies.size(); i++)
		LoadAssembly(CoreAssemblyDependencies[i], false /* Watch for changes */);

	s_CoreAssembly = LoadAssembly(s_CoreDLLPath, true, false);
	if (s_CoreAssembly)
		s_CoreAssembly->LoadScriptCoreTypes();
}

MonoDomain* ScriptEngine::GetAppDomain() { return s_AppDomain; }
bool ScriptEngine::IsLoaded() { return s_RootDomain != nullptr; }

void ScriptEngine::OnAssemblyFileChanged(const std::string& path, IO::FileWatchStatus status)
{
	spdlog::debug("Assembly '{}' was changed, flagging script engine as ready to reload", path);
	s_AwaitingReload = true;
}

bool ScriptEngine::AwaitingReload() { return s_AwaitingReload; }
void ScriptEngine::SetAwaitingReload() { s_AwaitingReload = true; }
bool ScriptEngine::DebuggingEnabled() { return s_DebuggingEnabled; }

void ScriptEngine::Reload(bool force)
{
	if (!s_AwaitingReload && !force)
		return;
	s_AwaitingReload = false;

	Timer timer;
	timer.Start();

	// Call pre-reload callbacks
	for (const function<void()>& callback : s_PreReloadCallbacks)
		callback();

	// Call OnDisable & OnDestroyed in all managed components
	SceneSystem* sceneSystem = SystemManager::Global()->Get<SceneSystem>();
	vector<World*> worlds = World::GetWorlds();

	for (World* world : worlds)
	{
		world->GetSystemManager()->InvalidateAllManagedInstances();
		world->GetComponentManager()->InvalidateAllManagedInstances();
	}
	SystemManager::Global()->InvalidateAllManagedInstances();

	// Release resources
	mono_domain_set(mono_get_root_domain(), true);
	mono_domain_unload(s_AppDomain);

	for (Assembly* instance : s_Assemblies)
		delete instance;
	s_Assemblies.clear();
	Assembly::ClearCachedTypes();

	// Restart mono
	s_AppDomain = mono_domain_create_appdomain((char*)AppDomainName, nullptr);
	mono_domain_set(s_AppDomain, true);

	std::vector<AssemblyPath> assemblyPaths = s_AssemblyPaths;
	s_AssemblyPaths.clear();

	// Load AquaScriptCore assembly
	LoadCoreAssembly();

	// Load all previously loaded assemblies, in same order
	for (AssemblyPath& assemblyPath : assemblyPaths)
	{
		if(!VFS::Exists(assemblyPath.Path))
		{
			spdlog::warn("Assembly '{}' no longer exists in filesystem", assemblyPath.Path);
			// TODO: Check if continue watching assembly that has been removed causes issues.
			// File may be restored later, it is desirable to be notified if it does.
			continue;
		}

		spdlog::debug("Reloading assembly '{}' {}", assemblyPath.Path, assemblyPath.WatchForChanges ? " (watching for changes)" : "");
		if (assemblyPath.WatchForChanges)
			VFS::GetMapping(assemblyPath.Path)->Unwatch(assemblyPath.Path);
		LoadAssembly(assemblyPath.Path, assemblyPath.WatchForChanges);
	}

	spdlog::debug("Loaded scripting assemblies in {}ms", timer.ElapsedTime().count());

	SystemManager::Global()->CreateAllManagedInstances();
	worlds = World::GetWorlds(); // Refresh worlds, as they can be created or destroyed with C# scripts
	for (World* world : worlds)
		world->GetSystemManager()->CreateAllManagedInstances();

	for (const function<void()>& callback : s_ReloadCallbacks)
		callback();

	timer.Stop();
	spdlog::debug("Reloaded scripting engine in {}ms", timer.ElapsedTime().count());
}

/// <returns>The managed type with matching hash, or nullptr if not found in any loaded assembly</returns>
MonoType* ScriptEngine::GetTypeFromHash(size_t hash)
{
	MonoType* type = nullptr;
	for (Assembly* assembly : s_Assemblies)
	{
		if ((type = assembly->GetTypeFromHash(hash)))
			break;
	}
	return type;
}

void ScriptEngine::AddReloadCallback(function<void()> callback)
{ s_ReloadCallbacks.emplace_back(callback); }

void ScriptEngine::AddPreReloadCallback(function<void()> callback)
{ s_PreReloadCallbacks.emplace_back(callback); }

void ScriptEngine::AddInternalCall(const char* name, const void* fn)
{ mono_add_internal_call(name, fn); }

void ScriptEngine::AddInternalCalls(const vector<pair<const char*, const void*>>& methods)
{
	for (auto pair : methods)
		AddInternalCall(pair.first, pair.second);
}
