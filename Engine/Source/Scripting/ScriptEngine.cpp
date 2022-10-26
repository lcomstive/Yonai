#include <spdlog/spdlog.h>
#include <AquaEngine/Timer.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <mono/metadata/assembly.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Scripting;

const char* AssembliesPath = "/Assets/Scripts/";
const char* AppDomainName = "AquaEngineAppDomain";

ScriptEngine::ScriptEngine(std::string& coreDllPath) : m_CoreDLLPath(coreDllPath)
{
	if (m_RootDomain)
		return; // Already initialised

	string assembliesPath = VFS::GetAbsolutePath(AssembliesPath);
	if (!VFS::Exists(assembliesPath))
	{
		spdlog::error("Mono assemblies path is invalid, disabling scripting");
		return;
	}
	mono_set_assemblies_path(assembliesPath.c_str());

	m_RootDomain = mono_jit_init("AquaEngineRuntime");
	if (!m_RootDomain)
	{
		spdlog::critical("Failed to create mono domain");
		return;
	}

	m_AppDomain = mono_domain_create_appdomain((char*)AppDomainName, nullptr);
	mono_domain_set(m_AppDomain, true);

	LoadCoreAssembly();
}

ScriptEngine::~ScriptEngine()
{
	if (!m_RootDomain)
		return; // Already destroyed

	mono_jit_cleanup(m_RootDomain);

	m_RootDomain = nullptr;
	m_AppDomain = nullptr;
}

Assembly* ScriptEngine::LoadAssembly(string path) { return LoadAssembly(path, true); }

Assembly* ScriptEngine::LoadAssembly(string& path, bool watch)
{
	if(path.empty() || !IO::VFS::Exists(path))
	{
		spdlog::warn("Failed to load '{}' - could not be found", path);
		return nullptr;
	}

	spdlog::debug("Loading C# script from '{}'", path);
	auto data = IO::VFS::Read(path);
	if(data.empty())
	{
		spdlog::warn("Failed to load '{}' - failed to read", path);
		return nullptr;
	}

	#ifndef NDEBUG
	if(watch)
	{
		// Debug mode, watch files and reload if any changes occur
		m_AssemblyPaths.push_back(path);

		IO::VFS::GetMapping(path)->Watch(path, std::bind(&ScriptEngine::OnAssemblyFileChanged, this, ::placeholders::_1, ::placeholders::_2));
	}
	#endif

	MonoImageOpenStatus status;
	MonoImage* image = mono_image_open_from_data_full(
		reinterpret_cast<char*>(data.data()),
		(uint32_t)data.size(),
		1, 	// Bool. Copy data to internal mono buffer
		&status,
		0	// Bool. Load in reflection mode
	);

	if(status != MONO_IMAGE_OK)
	{
		spdlog::error("Failed to load '{}' - '{}'", path, mono_image_strerror(status));
		return nullptr;
	}

	MonoAssembly* assembly = mono_assembly_load_from_full(
		image,
		path.c_str(), // Friendly name, used for warnings & errors
		&status,
		0	// Bool. Load in reflection mode
	);
	mono_image_close(image);

	Assembly* instance = new Assembly(this, assembly);
	m_Assemblies.push_back(instance);
	return instance;
}

void ScriptEngine::LoadCoreAssembly()
{
	Assembly* coreAssembly = LoadAssembly(m_CoreDLLPath, false);
	if(coreAssembly)
		coreAssembly->LoadScriptCoreTypes();
}

MonoDomain* ScriptEngine::GetAppDomain() { return m_AppDomain; }

#ifndef NDEBUG
void ScriptEngine::OnAssemblyFileChanged(const std::string& path, IO::FileWatchStatus status)
{
	spdlog::debug("Assembly '{}' was changed, flagging script engine as ready to reload", path);
	m_AwaitingReload = true;
}
#endif

bool ScriptEngine::AwaitingReload() { return m_AwaitingReload; }

void ScriptEngine::Reload(bool force)
{
#ifndef NDEBUG
	if(!m_AwaitingReload && !force)
		return;
	m_AwaitingReload = false;

	Timer timer;
	timer.Start();

	// Release resources
	mono_domain_set(mono_get_root_domain(), false);
	mono_domain_unload(m_AppDomain);
	
	for(Assembly* instance : m_Assemblies)
		delete instance;
	m_Assemblies.clear();

	// Restart mono
	m_AppDomain = mono_domain_create_appdomain((char*)AppDomainName, nullptr);
	mono_domain_set(m_AppDomain, true);

	std::vector<string> assemblyPaths = m_AssemblyPaths;
	m_AssemblyPaths.clear();

	LoadCoreAssembly();

	// Load all previously loaded assemblies, in same order
	for(string& assemblyPath : assemblyPaths)
	{
		spdlog::debug("Reloading assembly {}", assemblyPath);
		VFS::GetMapping(assemblyPath)->Unwatch(assemblyPath);
		LoadAssembly(assemblyPath);
	}

	spdlog::debug("Reloaded scripting engine in {}ms", timer.ElapsedTime().count());
#endif
}