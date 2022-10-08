#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
#include <mono/metadata/assembly.h>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

const char* AppDomainName = "AquaEngineAppDomain";

ScriptSystem::ScriptSystem()
{
	mono_set_assemblies_path("mono/lib/4.5");

	m_RootDomain = mono_jit_init("AquaEngineRuntime");
	if(!m_RootDomain)
	{
		spdlog::critical("Failed to create mono domain");
		return;
	}

	m_AppDomain = mono_domain_create_appdomain((char*)AppDomainName, nullptr);
	mono_domain_set(m_AppDomain, true);

	AddInternalCalls();
}

ScriptSystem::~ScriptSystem()
{
	mono_jit_cleanup(m_RootDomain);
}

void PrintAssemblyTypes(MonoAssembly* assembly)
{
	MonoImage* image = mono_assembly_get_image(assembly);
	const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
	int typeCount = mono_table_info_get_rows(typeDefinitionsTable);

	spdlog::debug("Assembly types for {}:", mono_assembly_name_get_name(mono_assembly_get_name(assembly)));
	for (int i = 0; i < typeCount; i++)
	{
		uint32_t columns[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typeDefinitionsTable, i, columns, MONO_TYPEDEF_SIZE);

		spdlog::debug("  {}.{}",
			mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAMESPACE]),
			mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAME])
		);
	}
}

unique_ptr<Assembly> ScriptSystem::LoadAssembly(const string& path)
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

	if(assembly)
		PrintAssemblyTypes(assembly);

	return make_unique<Assembly>(this, assembly);
}

MonoDomain* ScriptSystem::GetAppDomain() { return m_AppDomain; }

MonoString* GetMessage() { return mono_string_new(mono_domain_get(), "Hello from C++"); }

void ScriptSystem::AddInternalCalls()
{
	mono_add_internal_call("Sample::GetMessage", GetMessage);
}