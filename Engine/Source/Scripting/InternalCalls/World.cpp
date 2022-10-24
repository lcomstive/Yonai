#include <memory>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/ScriptComponent.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

extern void (*ComponentMethodInitialise)(MonoObject*, unsigned int, unsigned int, MonoException**);
extern void (*ComponentMethodStart)(MonoObject*, MonoException**);
extern void (*ComponentMethodUpdate)(MonoObject*, MonoException**);
extern void (*ComponentMethodOnDestroy)(MonoObject*, MonoException**);

#pragma region World
bool WorldGet(unsigned int worldID, MonoString** outName)
{
	World* world = World::GetWorld(worldID);
	if (!world)
	{
		*outName = mono_string_new(mono_domain_get(), "");
		return false;
	}

	*outName = mono_string_new(mono_domain_get(), world->Name().c_str());
	return true;
}

bool WorldExists(unsigned int worldID)
{
	return World::GetWorld(worldID) != nullptr;
}

bool WorldDestroy(unsigned int worldID)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return false;
	world->Destroy();
	delete world;
	return true;
}

unsigned int WorldCreate(MonoString* name)
{
	World* world = new World(mono_string_to_utf8(name));
	return world->ID();
}

unsigned int WorldCreateEntity(unsigned int worldID)
{
	World* world = World::GetWorld(worldID);
	return world ? world->CreateEntity().ID() : InvalidEntityID;
}

void WorldDestroyEntity(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	if (world)
		world->DestroyEntity(entityID);
}

bool WorldHasEntity(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	return world ? world->HasEntity(entityID) : false;
}
#pragma endregion

#pragma region Entity
size_t GetComponentType(MonoReflectionType* componentType)
{
	MonoClass* klass = mono_type_get_class(mono_reflection_type_get_type(componentType));
	size_t managedType = Assembly::GetTypeHash(mono_class_get_type(klass));

	size_t type = Assembly::GetTypeHash(mono_class_get_type(klass));

	return Assembly::GetManagedComponentData(managedType).Type;
}

bool EntityHasComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	return world->GetComponentManager()->Has(entityID, GetComponentType(componentType));
}

MonoObject* EntityGetComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType);

MonoObject* EntityAddComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;

	MonoType* managedType = mono_reflection_type_get_type(componentType);
	MonoClass* klass = mono_type_get_class(managedType);
	Assembly::ManagedComponentData managedData = Assembly::GetManagedComponentData(Assembly::GetTypeHash(mono_class_get_type(klass)));

	spdlog::debug("Trying to add component of type '{}' [{}]", mono_type_get_name(mono_reflection_type_get_type(componentType)), managedData.Type);

	// Check for existing component of type
	if (world->GetComponentManager()->Has(entityID, managedData.Type))
		return EntityGetComponent(worldID, entityID, componentType);

	Components::Component* unmanagedInstance = nullptr;

	if (!managedData.AddFn)
	{
		unmanagedInstance = world->AddComponent<Components::ScriptComponent>(entityID, managedData.Type);
		((Components::ScriptComponent*)unmanagedInstance)->Type = managedType;
	}
	else
		unmanagedInstance = managedData.AddFn(world, entityID);

	unmanagedInstance->ManagedInstance = mono_object_new(mono_domain_get(), klass);

	// Call initialise method, setting entity & world IDs in the component
	MonoException* exception = nullptr;
	unsigned int params[2] = { worldID, entityID };
	ComponentMethodInitialise(unmanagedInstance->ManagedInstance, worldID, entityID, &exception);

	// Call constructor
	mono_runtime_object_init(unmanagedInstance->ManagedInstance);

	return unmanagedInstance->ManagedInstance;
}

bool EntityRemoveComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	size_t type = GetComponentType(componentType);
	Components::Component* component = (Components::Component*)world->GetComponentManager()->Get(entityID, type);
	if (component && component->ManagedInstance)
	{
		MonoException* exception = nullptr;
		ComponentMethodOnDestroy(component->ManagedInstance, &exception);
	}

	return world->GetComponentManager()->Remove(entityID, type);
}

MonoObject* EntityGetComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	MonoClass* klass = mono_type_get_class(mono_reflection_type_get_type(componentType));
	size_t type = Assembly::GetTypeHash(mono_class_get_type(klass));

	Components::Component* instance = (Components::Component*)world->GetComponent(entityID, type);

	if (!instance)
		return nullptr;

	if (!instance->ManagedInstance)
	{
		// Unmanaged (C++) type
		instance->ManagedInstance = mono_object_new(mono_domain_get(), klass);

		// Call initialise method
		MonoException* exception = nullptr;
		unsigned int params[2] = { worldID, entityID };
		ComponentMethodInitialise(instance->ManagedInstance, worldID, entityID, &exception);
	}
	return instance->ManagedInstance;
}
#pragma endregion
/// <summary>
/// Maps an unmanaged (C++) component type to a managed (C#) component
/// </summary>
template<typename T>
void AddInternalManagedComponent(char* managedNamespace, char* managedName, MonoImage* image)
{
	MonoClass* klass = mono_class_from_name(image, managedNamespace, managedName);
	if (!klass)
	{
		spdlog::warn("Failed to add internal managed component definition for '{}' - not found in assembly '{}'", managedName, mono_image_get_name(image));
		return;
	}

	s_InternalManagedComponentTypes.emplace(
		Assembly::GetTypeHash(mono_class_get_type(klass)),
		ManagedComponentData
		{
			typeid(T).hash_code(),
			[](World* world, EntityID entityID) -> Components::Component* { return world->AddComponent<T>(entityID); }
		}
	);
}

void AquaEngine::Scripting::Assembly::AddWorldInternalCalls()
{
	mono_add_internal_call("AquaEngine.World::_aqua_internal_World_Get", (const void*)WorldGet);
	mono_add_internal_call("AquaEngine.World::_aqua_internal_World_Exists", (const void*)WorldExists);
	mono_add_internal_call("AquaEngine.World::_aqua_internal_World_Create", (const void*)WorldCreate);
	mono_add_internal_call("AquaEngine.World::_aqua_internal_World_Destroy", (const void*)WorldDestroy);
	mono_add_internal_call("AquaEngine.World::_aqua_internal_World_HasEntity", (const void*)WorldHasEntity);
	mono_add_internal_call("AquaEngine.World::_aqua_internal_World_CreateEntity", (const void*)WorldCreateEntity);
	mono_add_internal_call("AquaEngine.World::_aqua_internal_World_DestroyEntity", (const void*)WorldCreateEntity);

	mono_add_internal_call("AquaEngine.Entity::_aqua_internal_Entity_HasComponent", (const void*)EntityHasComponent);
	mono_add_internal_call("AquaEngine.Entity::_aqua_internal_Entity_GetComponent", (const void*)EntityGetComponent);
	mono_add_internal_call("AquaEngine.Entity::_aqua_internal_Entity_AddComponent", (const void*)EntityAddComponent);
	mono_add_internal_call("AquaEngine.Entity::_aqua_internal_Entity_RemoveComponent", (const void*)EntityRemoveComponent);
}