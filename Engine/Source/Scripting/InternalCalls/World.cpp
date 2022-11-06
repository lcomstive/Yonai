#include <memory>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Systems/ScriptSystem.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>
#include <AquaEngine/Components/ScriptComponent.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

extern EmptyMethodFn ComponentMethodStart;
extern EmptyMethodFn ComponentMethodUpdate;
extern EmptyMethodFn ComponentMethodDestroyed;
extern ComponentMethodEnabledFn ComponentMethodEnabled;
extern ComponentMethodInitialiseFn ComponentMethodInitialise;

extern EmptyMethodFn SystemMethodDestroyed;
extern SystemMethodEnabledFn SystemMethodEnabled;

#pragma region World
bool World_Get(unsigned int worldID, MonoString** outName)
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

bool World_Exists(unsigned int worldID)
{
	return World::GetWorld(worldID) != nullptr;
}

bool World_Destroy(unsigned int worldID)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return false;
	world->Destroy();
	delete world;
	return true;
}

unsigned int World_Create(MonoString* name)
{
	World* world = new World(mono_string_to_utf8(name));
	return world->ID();
}

unsigned int World_CreateEntity(unsigned int worldID)
{
	World* world = World::GetWorld(worldID);
	return world ? world->CreateEntity().ID() : InvalidEntityID;
}

void World_DestroyEntity(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	if (world)
		world->DestroyEntity(entityID);
}

bool World_HasEntity(unsigned int worldID, unsigned int entityID)
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

bool Entity_HasComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	return world->GetComponentManager()->Has(entityID, GetComponentType(componentType));
}

MonoObject* Entity_GetComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType);

MonoObject* Entity_AddComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;

	MonoType* managedType = mono_reflection_type_get_type(componentType);
	size_t typeHash = Assembly::GetTypeHash(managedType);

	// Check for existing component of same type on entity
	if (world->GetComponentManager()->Has(entityID, typeHash))
		return world->GetComponentManager()->Get(entityID, typeHash)->ManagedData.Instance;

	// Get unmanaged->managed data
	Assembly::ManagedComponentData managedData = Assembly::GetManagedComponentData(typeHash);
	if (!managedData.AddFn)
		return world->GetComponentManager()->Add(entityID, managedType)->ManagedData.Instance;
	return managedData.AddFn(world, entityID)->ManagedData.Instance;
}

bool Entity_RemoveComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	size_t type = GetComponentType(componentType);
	Components::Component* component = (Components::Component*)world->GetComponentManager()->Get(entityID, type);
	if (component && component->ManagedData.Instance)
	{
		// Call Disabled() and then Destroyed()
		MonoException* exception = nullptr;
		ComponentMethodEnabled(component->ManagedData.Instance, false, &exception);
		ComponentMethodDestroyed(component->ManagedData.Instance, &exception);
	}

	return world->GetComponentManager()->Remove(entityID, type);
}

MonoObject* Entity_GetComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	MonoType* managedType = mono_reflection_type_get_type(componentType);
	MonoClass* klass = mono_type_get_class(managedType);
	size_t type = Assembly::GetTypeHash(mono_class_get_type(klass));

	Components::Component* instance = (Components::Component*)world->GetComponent(entityID, type);
	return instance ? instance->ManagedData.Instance : nullptr;
}
#pragma endregion

#pragma region Systems
size_t GetSystemType(MonoReflectionType* componentType)
{
	MonoClass* klass = mono_type_get_class(mono_reflection_type_get_type(componentType));
	size_t managedType = Assembly::GetTypeHash(mono_class_get_type(klass));

	size_t type = Assembly::GetTypeHash(mono_class_get_type(klass));

	return Assembly::GetManagedSystemData(managedType).Type;
}

SystemManager* GetSystemManager(unsigned int worldID)
{
	if (worldID == InvalidEntityID)
		return SystemManager::Global();

	World* world = World::GetWorld(worldID);
	return world ? world->GetSystemManager() : nullptr;
}

bool World_HasSystem(unsigned int worldID, MonoReflectionType* systemType)
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return false;
	return systemManager->Has(GetSystemType(systemType));
}

MonoObject* World_GetSystem(unsigned int worldID, MonoReflectionType* systemType)
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return nullptr;

	MonoType* managedType = mono_reflection_type_get_type(systemType);
	MonoClass* klass = mono_type_get_class(managedType);
	size_t type = Assembly::GetTypeHash(mono_class_get_type(klass));

	Systems::System* instance = systemManager->Get(type);
	return instance ? instance->ManagedData.Instance : nullptr;
}

MonoObject* World_AddSystem(unsigned int worldID, MonoReflectionType* systemType)
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return nullptr;

	MonoType* managedType = mono_reflection_type_get_type(systemType);
	size_t typeHash = Assembly::GetTypeHash(managedType);

	// Check for existing component of same type on entity
	if (systemManager->Has(typeHash))
		return systemManager->Get(typeHash)->ManagedData.Instance;

	// Get unmanaged->managed data
	Assembly::ManagedSystemData systemData = Assembly::GetManagedSystemData(typeHash);
	if (!systemData.AddFn)
		return systemManager->Add(managedType)->ManagedData.Instance;
	return systemData.AddFn(systemManager)->ManagedData.Instance;
}

bool World_RemoveSystem(unsigned int worldID, MonoReflectionType* systemType)
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return false;

	size_t type = GetSystemType(systemType);
	Systems::System* system = systemManager->Get(type);
	if (system && system->ManagedData.Instance)
	{
		// Call Disabled and then Destroyed
		MonoException* exception = nullptr;
		SystemMethodEnabled(system->ManagedData.Instance, false, &exception);
		SystemMethodDestroyed(system->ManagedData.Instance, &exception);
	}

	return systemManager->Remove(type);
}

#pragma endregion

#define ADD_WORLD_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.World::_aqua_internal_"#name, (const void*)name);
#define ADD_ENTITY_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.Entity::_aqua_internal_"#name, (const void*)name);

void AquaEngine::Scripting::Assembly::AddWorldInternalCalls()
{
	ADD_WORLD_INTERNAL_CALL(World_Get)
	ADD_WORLD_INTERNAL_CALL(World_Exists)
	ADD_WORLD_INTERNAL_CALL(World_Create)
	ADD_WORLD_INTERNAL_CALL(World_Destroy)
	ADD_WORLD_INTERNAL_CALL(World_HasEntity)
	ADD_WORLD_INTERNAL_CALL(World_CreateEntity)
	ADD_WORLD_INTERNAL_CALL(World_DestroyEntity)

	ADD_WORLD_INTERNAL_CALL(World_HasSystem)
	ADD_WORLD_INTERNAL_CALL(World_GetSystem)
	ADD_WORLD_INTERNAL_CALL(World_AddSystem)
	ADD_WORLD_INTERNAL_CALL(World_RemoveSystem)

	ADD_ENTITY_INTERNAL_CALL(Entity_HasComponent)
	ADD_ENTITY_INTERNAL_CALL(Entity_GetComponent)
	ADD_ENTITY_INTERNAL_CALL(Entity_AddComponent)
	ADD_ENTITY_INTERNAL_CALL(Entity_RemoveComponent)
}