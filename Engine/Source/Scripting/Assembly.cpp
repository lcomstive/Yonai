#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>

// Components to map, unmanaged -> managed
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/Transform.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

unordered_map<MonoType*, size_t> Assembly::s_TypeHashes;
unordered_map<size_t, Assembly::ManagedComponentData> Assembly::s_InternalManagedComponentTypes = {};

Assembly::Assembly(ScriptEngine* owner, MonoAssembly* handle) : Owner(owner), Handle(handle), Image(mono_assembly_get_image(handle))
{
	HashTypes();
	AddInternalCalls();
}

unique_ptr<Class> Assembly::InstantiateClass(const char* namespaceName, const char* className)
{
	MonoImage* image = mono_assembly_get_image(Handle);
	MonoClass* klass = mono_class_from_name(image, namespaceName, className);

	if(!klass)
	{
		spdlog::warn("Failed to load C# class '{}.{}'", namespaceName, className);
		return nullptr;
	}

	MonoObject* instance = mono_object_new(Owner->GetAppDomain(), klass);

	if(!instance)
	{
		spdlog::error("Failed to create instance of '{}.{}'", namespaceName, className);
		return nullptr;
	}

	// Call constructor
	mono_runtime_object_init(instance);

	return make_unique<Class>(klass, instance);
}

size_t Assembly::GetTypeHash(MonoType* type)
{
	auto it = s_TypeHashes.find(type);
	if (it == s_TypeHashes.end())
		return 0;
	auto managedIt = s_InternalManagedComponentTypes.find(it->second);
	if (managedIt != s_InternalManagedComponentTypes.end())
		return managedIt->second.Type;
	return it->second;
}

Assembly::ManagedComponentData Assembly::GetManagedComponentData(size_t unmanagedType)
{
	auto it = s_InternalManagedComponentTypes.find(unmanagedType);
	return it == s_InternalManagedComponentTypes.end() ? Assembly::ManagedComponentData{ unmanagedType } : it->second;
}

void Assembly::HashTypes()
{
	const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(Image, MONO_TABLE_TYPEDEF);
	int typeCount = mono_table_info_get_rows(typeDefinitionsTable);

	spdlog::debug("Assembly types for {}:", mono_assembly_name_get_name(mono_assembly_get_name(Handle)));
	for (int i = 0; i < typeCount; i++)
	{
		uint32_t columns[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typeDefinitionsTable, i, columns, MONO_TYPEDEF_SIZE);

		const char* _name = mono_metadata_string_heap(Image, columns[MONO_TYPEDEF_NAME]);
		const char* _namespace = mono_metadata_string_heap(Image, columns[MONO_TYPEDEF_NAMESPACE]);
		MonoClass* klass = mono_class_from_name(Image, _namespace, _name);

		string fullName(_namespace);
		fullName += ".";
		fullName += _name;
		size_t hash = std::hash<std::string>{}(fullName);

		spdlog::debug("  {}.{} [{}]", _namespace, _name, hash);

		s_TypeHashes.emplace(mono_class_get_type(klass), hash);
	}
}

void Assembly::AddInternalCalls()
{
	AddLogInternalCalls();
	AddTimeInternalCalls();
	AddWorldInternalCalls();
	AddVectorInternalCalls();
	AddTransformInternalCalls();
}

void (*ComponentMethodStart)(MonoObject*, MonoException**) = nullptr;
void (*ComponentMethodUpdate)(MonoObject*, MonoException**) = nullptr;
void (*ComponentMethodDestroy)(MonoObject*, MonoException**) = nullptr;
void (*ComponentMethodEnable)(MonoObject*, bool, MonoException**) = nullptr;
void (*ComponentMethodInitialise)(MonoObject*, unsigned int, unsigned int, MonoException**) = nullptr;

#define AddComponentMethod(name) \
	method = mono_class_get_method_from_name(component, "_"#name, 0); \
	if(ComponentMethod##name == nullptr && method) \
		ComponentMethod##name = (void(*)(MonoObject*, MonoException**))mono_method_get_unmanaged_thunk(method);

void Assembly::LoadScriptCoreTypes()
{
	AddInternalManagedComponent<Components::Camera>("AquaEngine", "Camera");
	AddInternalManagedComponent<Components::Transform>("AquaEngine", "Transform");

	MonoClass* component = mono_class_from_name(Image, "AquaEngine", "Component");

	MonoMethod* method = mono_class_get_method_from_name(component, "Initialise", 2);
	if (ComponentMethodInitialise == nullptr && method)
		ComponentMethodInitialise = (void(*)(MonoObject*, unsigned int, unsigned int, MonoException**))mono_method_get_unmanaged_thunk(method);

	method = mono_class_get_method_from_name(component, "_Enable", 1);
	if (ComponentMethodEnable == nullptr && method)
		ComponentMethodEnable = (void(*)(MonoObject*, bool, MonoException**))mono_method_get_unmanaged_thunk(method);

	AddComponentMethod(Start)
	AddComponentMethod(Update)
	AddComponentMethod(Destroy)
}
