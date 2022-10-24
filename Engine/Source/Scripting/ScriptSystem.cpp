#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
#include <mono/metadata/assembly.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Scripting;

const char* AssembliesPath = "/Assets/Scripts/";
const char* AppDomainName = "AquaEngineAppDomain";

ScriptSystem::ScriptSystem()
{
	if (m_RootDomain)
		return; // Already initialised

	string assembliesPath = VFS::GetAbsolutePath(AssembliesPath);
	if (!VFS::Exists(assembliesPath))
	{
		spdlog::error("Mono assemblies path is invalid, disabling scripting");
		GetManager()->Remove<ScriptSystem>();
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
}

ScriptSystem::~ScriptSystem()
{
	if (!m_RootDomain)
		return; // Already destroyed

	mono_jit_cleanup(m_RootDomain);

	m_RootDomain = nullptr;
	m_AppDomain = nullptr;
}

unique_ptr<Assembly> ScriptSystem::LoadAssembly(string& path)
{
	spdlog::debug("Loading C# script from '{}'", path);
	auto data = IO::VFS::Read(path);

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

	return make_unique<Assembly>(this, assembly);
}

MonoDomain* ScriptSystem::GetAppDomain() { return m_AppDomain; }
