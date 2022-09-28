#include <Scripting.hpp>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>

using namespace std;
using namespace AquaEditor;
using namespace AquaEngine::IO;

MonoDomain* ScriptEngine::AppDomain = nullptr;
MonoDomain* ScriptEngine::RootDomain = nullptr;

void ScriptEngine::InitMono()
{
	mono_set_assemblies_path("mono/lib/4.5");

	// Create domain
	RootDomain = mono_jit_init("MyScriptRuntime");
	if (!RootDomain)
	{
		spdlog::critical("Failed to create mono domain");
		return;
	}

	// Create app domain
	AppDomain = mono_domain_create_appdomain("MyAppDomain", nullptr);
	mono_domain_set(AppDomain, true);
}

void PrintAssemblyTypes(MonoAssembly* assembly);

unique_ptr<Assembly> ScriptEngine::LoadAssembly(const std::string& path)
{
	spdlog::debug("Loading C# script from '{}'", path);
	auto data = VFS::Read(path);

	MonoImageOpenStatus status;
	MonoImage* image = mono_image_open_from_data_full(
		reinterpret_cast<char*>(data.data()),
		(uint32_t)data.size(),
		1, // Bool. Copy data to internal mono buffer
		&status,
		0  // Bool. Load in reflection mode
	);

	if (status != MONO_IMAGE_OK)
	{
		spdlog::error("Failed to load '{}' - {}", path, mono_image_strerror(status));
		return nullptr;
	}

	MonoAssembly* assembly = mono_assembly_load_from_full(
		image,
		path.c_str(), // Friendly name, used for warnings & errors
		&status,
		0 // Bool. Load in reflection mode
	);
	mono_image_close(image);

	PrintAssemblyTypes(assembly);

	return make_unique<Assembly>(assembly);
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

MonoClass* ScriptEngine::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
	MonoImage* image = mono_assembly_get_image(assembly);
	MonoClass* klass = mono_class_from_name(image, namespaceName, className);

	if (klass)
		return klass;

	spdlog::warn("Failed to load C# class '{}.{}'", namespaceName, className);
	return nullptr;
}

Assembly::Assembly(MonoAssembly* handle) : Handle(handle) { }

unique_ptr<Class> Assembly::InstantiateClass(const char* namespaceName, const char* className)
{
	MonoClass* klass = ScriptEngine::GetClassInAssembly(Handle, namespaceName, className);
	MonoObject* instance = mono_object_new(ScriptEngine::AppDomain, klass);

	if (!instance)
	{
		spdlog::error("Failed to create instance of '{}.{}'", namespaceName, className);
		return nullptr;
	}

	mono_runtime_object_init(instance);
	return make_unique<Class>(klass, instance);
}

Class::Class(MonoClass* handle, MonoObject* instance) : Handle(handle), Instance(instance) { }

MonoMethod* Class::GetMethod(const char* name, int parameterCount)
{ return mono_class_get_method_from_name(Handle, name, parameterCount); }

MonoClassField* Class::GetField(const char* name)
{ return mono_class_get_field_from_name(Handle, name); }

MonoProperty* Class::GetProperty(const char* name)
{ return mono_class_get_property_from_name(Handle, name); }
