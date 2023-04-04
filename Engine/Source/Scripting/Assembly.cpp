#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>

// Components to map, unmanaged -> managed
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Components/SoundSource.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

// Systems to map, unmanaged -> managed
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

unordered_map<MonoType*, size_t> Assembly::s_TypeHashes = {};
unordered_map<size_t, MonoType*> Assembly::s_ReverseTypeHashes = {};
unordered_map<size_t, Assembly::ManagedSystemData> Assembly::s_InternalManagedSystemTypes = {};
unordered_map<size_t, Assembly::ManagedComponentData> Assembly::s_InternalManagedComponentTypes = {};

// Unmanaged thunks definition
EmptyMethodFn ComponentMethodStart = nullptr;
EmptyMethodFn ComponentMethodUpdate = nullptr;
EmptyMethodFn ComponentMethodDestroyed = nullptr;
ComponentMethodEnabledFn ComponentMethodEnabled = nullptr;
ComponentMethodInitialiseFn ComponentMethodInitialise = nullptr;

EmptyMethodFn SystemMethodDraw = nullptr;
EmptyMethodFn SystemMethodStart = nullptr;
EmptyMethodFn SystemMethodUpdate = nullptr;
EmptyMethodFn SystemMethodDestroyed = nullptr;
SystemMethodEnabledFn SystemMethodEnabled = nullptr;
SystemMethodInitialiseFn SystemMethodInitialise = nullptr;

Assembly::Assembly(MonoAssembly* handle, bool isCoreAssembly) : Handle(handle), Image(mono_assembly_get_image(handle))
{ CacheTypes(isCoreAssembly); }

MonoClass* Assembly::GetClassFromName(const char* namespaceName, const char* className)
// { return mono_class_from_name(Image, namespaceName, className); }
{
	// Get type hash
	string fullName = strlen(namespaceName) == 0 ? className : fmt::format("{}.{}", namespaceName, className);
	size_t hash = std::hash<std::string>{}(fullName);

	return s_ReverseTypeHashes.find(hash) == s_ReverseTypeHashes.end() ?
		nullptr : mono_class_from_mono_type(s_ReverseTypeHashes[hash]);
}

MonoType* Assembly::GetTypeFromClassName(const char* namespaceName, const char* className)
{
	MonoClass* klass = GetClassFromName(namespaceName, className);
	return klass ? mono_class_get_type(klass) : nullptr;
}

MonoType* Assembly::GetTypeFromHash(size_t hash)
{
	auto it = s_ReverseTypeHashes.find(hash);
	return it == s_ReverseTypeHashes.end() ? nullptr : it->second;
}

unique_ptr<Class> Assembly::InstantiateClass(const char* namespaceName, const char* className)
{
	MonoClass* klass = mono_class_from_name(Image, namespaceName, className);

	if(!klass)
	{
		spdlog::warn("Failed to load C# class '{}.{}'", namespaceName, className);
		return nullptr;
	}

	MonoObject* instance = mono_object_new(ScriptEngine::GetAppDomain(), klass);

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

size_t Assembly::GetTypeHash(std::type_index& type) { return type.hash_code(); }
size_t Assembly::GetTypeHash(MonoClass* monoClass) { return GetTypeHash(mono_class_get_type(monoClass)); }

Assembly::ManagedComponentData Assembly::GetManagedComponentData(size_t unmanagedType)
{
	// Check with input type
	auto it = s_InternalManagedComponentTypes.find(unmanagedType);
	if (it != s_InternalManagedComponentTypes.end())
		return it->second;

	// Lookup type compared to managed types
	auto reverseIt = s_ReverseTypeHashes.find(unmanagedType);
	if (reverseIt != s_ReverseTypeHashes.end())
	{
		unmanagedType = s_TypeHashes[reverseIt->second];
		it = s_InternalManagedComponentTypes.find(unmanagedType);
		if (it != s_InternalManagedComponentTypes.end())
			return it->second;
	}

	// Not found, return empty
	return Assembly::ManagedComponentData { unmanagedType };
}

Assembly::ManagedSystemData Assembly::GetManagedSystemData(size_t unmanagedType)
{
	// Check with input type
	auto it = s_InternalManagedSystemTypes.find(unmanagedType);
	if (it != s_InternalManagedSystemTypes.end())
		return it->second;

	// Lookup type compared to managed types
	auto reverseIt = s_ReverseTypeHashes.find(unmanagedType);
	if (reverseIt != s_ReverseTypeHashes.end())
	{
		unmanagedType = s_TypeHashes[reverseIt->second];
		it = s_InternalManagedSystemTypes.find(unmanagedType);
		if (it != s_InternalManagedSystemTypes.end())
			return it->second;
	}

	// Not found, return empty
	return Assembly::ManagedSystemData { unmanagedType };
}

void Assembly::ClearCachedTypes()
{
	s_TypeHashes.clear();
	s_ReverseTypeHashes.clear();
}

size_t Assembly::GetUnmanagedHash(size_t managedHash)
{
	return s_InternalManagedComponentTypes.find(managedHash) == s_InternalManagedComponentTypes.end() ?
		managedHash : s_InternalManagedComponentTypes[managedHash].Type;
}

void Assembly::CacheTypes(bool isCore)
{
	Assembly* coreAssembly = isCore ? this : ScriptEngine::GetCoreAssembly();
	if (!coreAssembly)
		return;

	const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(Image, MONO_TABLE_TYPEDEF);
	int typeCount = mono_table_info_get_rows(typeDefinitionsTable);

	MonoClass* coreComponentType = mono_class_from_name(coreAssembly->Image, "AquaEngine", "Component");
	MonoClass* coreSystemType    = mono_class_from_name(coreAssembly->Image, "AquaEngine", "AquaSystem");

	// spdlog::trace("Assembly types for {}:", mono_assembly_name_get_name(mono_assembly_get_name(Handle)));
	for (int i = 0; i < typeCount; i++)
	{
		// Get metadata
		uint32_t columns[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typeDefinitionsTable, i, columns, MONO_TYPEDEF_SIZE);

		// Get namespace, name & class pointer
		const char* _name = mono_metadata_string_heap(Image, columns[MONO_TYPEDEF_NAME]);
		const char* _namespace = mono_metadata_string_heap(Image, columns[MONO_TYPEDEF_NAMESPACE]);
		MonoClass* klass = mono_class_from_name(Image, _namespace, _name);

		// Hash the type
		string fullName = strlen(_namespace) == 0 ? _name : fmt::format("{}.{}", _namespace, _name);
		size_t hash = std::hash<std::string>{}(fullName);

		if (!klass)
		{
			spdlog::warn("Failed to cache type '{}.{}'", _namespace, _name);
			continue;
		}

		// Check if derives from AquaEngine.System
		if (klass != coreSystemType && mono_class_is_subclass_of(klass, coreSystemType, false))
		{
			m_ManagedSystemTypes.push_back(klass);
			// spdlog::trace("  {}.{} [System][{}]", _namespace, _name, hash);
		}
		// Check if derives from AquaEngine.Component
		else if (klass != coreComponentType && mono_class_is_subclass_of(klass, coreComponentType, false))
		{
			m_ManagedComponentTypes.push_back(klass);
			// spdlog::trace("  {}.{} [Component][{}]", _namespace, _name, hash);
		}
		// else
			// spdlog::trace("  {}.{} [{}]", _namespace, _name, hash);

		MonoType* monoType = mono_class_get_type(klass);
		s_TypeHashes.emplace(monoType, hash);
		s_ReverseTypeHashes.emplace(hash, monoType);
	}
}

#define AddComponentMethod(name) \
	method = mono_class_get_method_from_name(component, #name, 0); \
	ComponentMethod##name = method ? (EmptyMethodFn)mono_method_get_unmanaged_thunk(method) : nullptr;

#define AddSystemMethod(name) \
	method = mono_class_get_method_from_name(system, #name, 0); \
	SystemMethod##name = method ? (EmptyMethodFn)mono_method_get_unmanaged_thunk(method) : nullptr;

void Assembly::LoadScriptCoreTypes()
{
	AddInternalManagedComponent<Components::Camera>("AquaEngine", "Camera");
	AddInternalManagedComponent<Components::Transform>("AquaEngine", "Transform");
	AddInternalManagedComponent<Components::SoundSource>("AquaEngine", "SoundSource");
	AddInternalManagedComponent<Components::MeshRenderer>("AquaEngine", "MeshRenderer");
	AddInternalManagedComponent<Components::SpriteRenderer>("AquaEngine", "SpriteRenderer");

	AddInternalManagedSystem<Systems::SceneSystem>("AquaEngine", "SceneManager");
	AddInternalManagedSystem<Systems::ImGUISystem>("AquaEngine.Systems", "ImGUISystem");

#pragma region Component Methods
	MonoClass* component = mono_class_from_name(Image, "AquaEngine", "Component");

	// Component.Initialise
	MonoMethod* method = mono_class_get_method_from_name(component, "aqua_Initialise", 2);
	ComponentMethodInitialise = (ComponentMethodInitialiseFn)mono_method_get_unmanaged_thunk(method);

	// Component._Enable
	method = mono_class_get_method_from_name(component, "aqua_Enable", 1);
	ComponentMethodEnabled = method ? (ComponentMethodEnabledFn)mono_method_get_unmanaged_thunk(method) : nullptr;

	method = mono_class_get_method_from_name(component, "Update", 0);
	ComponentMethodUpdate = method ? (EmptyMethodFn)mono_method_get_unmanaged_thunk(method) : nullptr;

	AddComponentMethod(Start)
	AddComponentMethod(Update)
	AddComponentMethod(Destroyed)
#pragma endregion

#pragma region System Methods
	MonoClass* system = mono_class_from_name(Image, "AquaEngine", "AquaSystem");

	// System.aqua_Initialise
	method = mono_class_get_method_from_name(system, "aqua_Initialise", 1);
	SystemMethodInitialise = method ? (void(*)(MonoObject*, uint64_t, MonoException**))mono_method_get_unmanaged_thunk(method) : nullptr;

	// System.aqua_Enable
	method = mono_class_get_method_from_name(system, "aqua_Enable", 1);
	SystemMethodEnabled = method ? (void(*)(MonoObject*, bool, MonoException**))mono_method_get_unmanaged_thunk(method) : nullptr;

	AddSystemMethod(Draw)
	AddSystemMethod(Start)
	AddSystemMethod(Update)
	AddSystemMethod(Destroyed)
#pragma endregion
}
