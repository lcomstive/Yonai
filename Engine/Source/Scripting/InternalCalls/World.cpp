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

size_t GetComponentType(MonoReflectionType* componentType)
{
	MonoClass* klass = mono_type_get_class(mono_reflection_type_get_type(componentType));
	size_t managedType = Assembly::GetTypeHash(mono_class_get_type(klass));

	size_t type = Assembly::GetTypeHash(mono_class_get_type(klass));

	return Assembly::GetManagedComponentData(managedType).Type;
}

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

MonoArray* World_GetEntities(unsigned int worldID)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	auto entities = world->Entities();
	MonoArray* entityIDs = mono_array_new(mono_domain_get(), mono_get_uint32_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(entityIDs, unsigned int, i, entities[i].ID());
	return entityIDs;
}

MonoArray* World_GetComponents(unsigned int worldID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	vector<EntityID> entities = world->GetComponentManager()->GetEntities(GetComponentType(componentType));
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint32_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(output, unsigned int, i, entities[i]);
	return output;
}

MonoArray* World_GetComponentsMultiple(unsigned int worldID, MonoArray* inputTypes)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	vector<size_t> componentTypes;
	size_t inputTypesLength = mono_array_length(inputTypes);
	componentTypes.reserve(inputTypesLength);
	for (size_t i = 0; i < inputTypesLength; i++)
		componentTypes.emplace_back(GetComponentType(mono_array_get(inputTypes, MonoReflectionType*, i)));

	vector<EntityID> entities = world->GetComponentManager()->GetEntities(componentTypes);
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint32_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(output, unsigned int, i, entities[i]);
	return output;
}
#pragma endregion

#pragma region Entity
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
		return world->GetComponentManager()->Get(entityID, typeHash)->ManagedData.GetInstance();

	// Get unmanaged->managed data
	Assembly::ManagedComponentData managedData = Assembly::GetManagedComponentData(typeHash);
	if (!managedData.AddFn)
		return world->GetComponentManager()->Add(entityID, managedType)->ManagedData.GetInstance();
	return managedData.AddFn(world, entityID)->ManagedData.GetInstance();
}

bool Entity_RemoveComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType)
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	size_t type = GetComponentType(componentType);
	Components::Component* component = (Components::Component*)world->GetComponentManager()->Get(entityID, type);
	if (component && component->ManagedData.IsValid())
	{
		// Call Disabled() and then Destroyed()
		MonoException* exception = nullptr;
		MonoObject* instance = component->ManagedData.GetInstance();
		ComponentMethodEnabled(instance, false, &exception);
		ComponentMethodDestroyed(instance, &exception);
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
	return instance ? instance->ManagedData.GetInstance() : nullptr;
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
	return instance ? instance->ManagedData.GetInstance() : nullptr;
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
		return systemManager->Get(typeHash)->ManagedData.GetInstance();

	// Get unmanaged->managed data
	Assembly::ManagedSystemData systemData = Assembly::GetManagedSystemData(typeHash);
	if (!systemData.AddFn)
		return systemManager->Add(managedType)->ManagedData.GetInstance();
	return systemData.AddFn(systemManager)->ManagedData.GetInstance();
}

bool World_RemoveSystem(unsigned int worldID, MonoReflectionType* systemType)
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return false;

	size_t type = GetSystemType(systemType);
	Systems::System* system = systemManager->Get(type);
	if (system && system->ManagedData.IsValid())
	{
		// Call Disabled and then Destroyed
		MonoException* exception = nullptr;
		MonoObject* instance = system->ManagedData.GetInstance();
		SystemMethodEnabled(instance, false, &exception);
		SystemMethodDestroyed(instance, &exception);
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

	ADD_WORLD_INTERNAL_CALL(World_GetEntities)
	ADD_WORLD_INTERNAL_CALL(World_GetComponents)
	ADD_WORLD_INTERNAL_CALL(World_GetComponentsMultiple)

	ADD_ENTITY_INTERNAL_CALL(Entity_HasComponent)
	ADD_ENTITY_INTERNAL_CALL(Entity_GetComponent)
	ADD_ENTITY_INTERNAL_CALL(Entity_AddComponent)
	ADD_ENTITY_INTERNAL_CALL(Entity_RemoveComponent)
}