#include <spdlog/spdlog.h>
#include <AquaEngine/Timer.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <mono/metadata/threads.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

const char* AssembliesPath = "/Assets/Scripts/";
const char* AppDomainName = "AquaEngineAppDomain";

string ScriptEngine::s_CoreDLLPath = "";
bool ScriptEngine::s_AwaitingReload = false;
bool ScriptEngine::s_DebuggingEnabled = false;
MonoDomain* ScriptEngine::s_AppDomain = nullptr;
Assembly* ScriptEngine::s_CoreAssembly = nullptr;
MonoDomain* ScriptEngine::s_RootDomain = nullptr;
vector<Assembly*> ScriptEngine::s_Assemblies = {};

vector<ScriptEngine::AssemblyPath> ScriptEngine::s_AssemblyPaths = {};

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
		static const char* options[] = {
		  "--soft-breakpoints",
		  "--debugger-agent=transport=dt_socket,server=y,address=0.0.0.0:55555,suspend=n,loglevel=3,logfile=MonoDebugger.log"
		};
		mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);

		spdlog::debug("C# debugging is enabled");
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

	if (!isCoreAssembly && shouldWatch)
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
		// Try and load .pdb file next to .dll file
		std::filesystem::path pdbPath(path);
		pdbPath.replace_extension(".mdb");
		if (VFS::Exists(pdbPath.string()))
		{
			auto pdbContents = VFS::Read(pdbPath.string());
			mono_debug_open_image_from_memory(image, pdbContents.data(), (int)pdbContents.size());
		}
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

void ScriptEngine::LoadCoreAssembly()
{
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
bool ScriptEngine::DebuggingEnabled() { return s_DebuggingEnabled; }

void ScriptEngine::Reload(bool force)
{
	if (!s_AwaitingReload && !force)
		return;
	s_AwaitingReload = false;

	Timer timer;
	timer.Start();

	// Call OnDisable & OnDestroyed in all managed components
	SceneSystem* sceneSystem = SystemManager::Global()->Get<SceneSystem>();
	vector<World*> worlds = World::GetWorlds();
	sceneSystem->UnloadAllScenes();
	for (World* world : worlds)
	{
		world->GetSystemManager()->InvalidateAllManagedInstances();
		world->GetComponentManager()->InvalidateAllManagedInstances();
	}

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
		spdlog::debug("Reloading assembly {} {}", assemblyPath.Path, assemblyPath.WatchForChanges ? " (watching for changes)" : "");
		if (assemblyPath.WatchForChanges)
			VFS::GetMapping(assemblyPath.Path)->Unwatch(assemblyPath.Path);
		LoadAssembly(assemblyPath.Path, assemblyPath.WatchForChanges);
	}

	spdlog::debug("Loaded scripting assemblies in {}ms", timer.ElapsedTime().count());

	for (World* world : worlds)
	{
		world->GetSystemManager()->CreateAllManagedInstances();
		sceneSystem->AddScene(world);
	}

	timer.Stop();
	spdlog::debug("Reloaded scripting engine in {}ms", timer.ElapsedTime().count());
}

/// <returns>The managed type with matching hash, or nullptr if not found in any loaded assembly</returns>
MonoType* ScriptEngine::GetTypeFromHash(size_t hash)
{
	MonoType* type = nullptr;
	for (Assembly* assembly : s_Assemblies)
	{
		if (type = assembly->GetTypeFromHash(hash))
			break;
	}
	return type;
}