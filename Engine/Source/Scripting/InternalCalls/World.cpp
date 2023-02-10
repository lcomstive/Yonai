#include <memory>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Systems/ScriptSystem.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
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

size_t _GetComponentType(MonoReflectionType* componentType)
{ return Assembly::GetTypeHash(mono_reflection_type_get_type(componentType)); }

#pragma region World
ADD_MANAGED_METHOD(World, Get, bool, (unsigned int worldID, MonoString** outName))
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

ADD_MANAGED_METHOD(World, Exists, bool, (unsigned int worldID))
{ return World::GetWorld(worldID) != nullptr; }

ADD_MANAGED_METHOD(World, Destroy, bool, (unsigned int worldID))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return false;
	world->Destroy();
	delete world;
	return true;
}

ADD_MANAGED_METHOD(World, Create, bool, (MonoString* name))
{
	World* world = new World(mono_string_to_utf8(name));
	return world->ID();
}

ADD_MANAGED_METHOD(World, CreateEntity, unsigned int, (unsigned int worldID))
{
	World* world = World::GetWorld(worldID);
	return world ? world->CreateEntity().ID() : InvalidEntityID;
}

ADD_MANAGED_METHOD(World, DestroyEntity, void, (unsigned int worldID, unsigned int entityID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		world->DestroyEntity(entityID);
}

ADD_MANAGED_METHOD(World, HasEntity, bool, (unsigned int worldID, unsigned int entityID))
{
	World* world = World::GetWorld(worldID);
	return world ? world->HasEntity(entityID) : false;
}

ADD_MANAGED_METHOD(World, GetEntities, MonoArray*, (unsigned int worldID))
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

ADD_MANAGED_METHOD(World, GetComponents, MonoArray*, (unsigned int worldID, MonoReflectionType* componentType))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	vector<EntityID> entities = world->GetComponentManager()->GetEntities(_GetComponentType(componentType));
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint32_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(output, unsigned int, i, entities[i]);
	return output;
}

ADD_MANAGED_METHOD(World, GetComponentsMultiple, MonoArray*, (unsigned int worldID, MonoArray* inputTypes))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	vector<size_t> componentTypes;
	size_t inputTypesLength = mono_array_length(inputTypes);
	componentTypes.reserve(inputTypesLength);
	for (size_t i = 0; i < inputTypesLength; i++)
		componentTypes.emplace_back(_GetComponentType(mono_array_get(inputTypes, MonoReflectionType*, i)));

	vector<EntityID> entities = world->GetComponentManager()->GetEntities(componentTypes);
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint32_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(output, unsigned int, i, entities[i]);
	return output;
}
#pragma endregion

#pragma region Entity
ADD_MANAGED_METHOD(Entity, HasComponent, bool, (unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType))
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	return world->GetComponentManager()->Has(entityID, _GetComponentType(componentType));
}

ADD_MANAGED_METHOD(Entity, GetComponent, MonoObject*, (unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType));

ADD_MANAGED_METHOD(Entity, AddComponent, MonoObject*, (unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType))
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
		return world->AddComponent(entityID, managedType)->ManagedData.GetInstance();
	return managedData.AddFn(world, entityID)->ManagedData.GetInstance();
}

ADD_MANAGED_METHOD(Entity, RemoveComponent, bool, (unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType))
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	size_t type = _GetComponentType(componentType);
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

ADD_MANAGED_METHOD(Entity, GetComponent, MonoObject*, (unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType))
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

ADD_MANAGED_METHOD(World, HasSystem, bool, (unsigned int worldID, MonoReflectionType* systemType))
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return false;
	return systemManager->Has(GetSystemType(systemType));
}

ADD_MANAGED_METHOD(World, GetSystem, MonoObject*, (unsigned int worldID, MonoReflectionType* systemType))
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

ADD_MANAGED_METHOD(World, AddSystem, MonoObject*, (unsigned int worldID, MonoReflectionType* systemType))
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

ADD_MANAGED_METHOD(World, RemoveSystem, bool, (unsigned int worldID, MonoReflectionType* systemType))
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
