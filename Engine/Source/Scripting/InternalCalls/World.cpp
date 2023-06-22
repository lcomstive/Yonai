#include <memory>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/World.hpp>
#include <AquaEngine/ComponentManager.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Systems/ScriptSystem.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>
#include <AquaEngine/Components/ScriptComponent.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

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
ADD_MANAGED_METHOD(World, Get, bool, (uint64_t worldID, MonoString** outName))
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

ADD_MANAGED_METHOD(World, GetAll, void, (MonoArray** outWorldIDs))
{
	vector<World*> worlds = World::GetWorlds();

	*outWorldIDs = mono_array_new(mono_domain_get(), mono_get_uint64_class(), worlds.size());
	for (size_t i = 0; i < worlds.size(); i++)
		mono_array_set(*outWorldIDs, uint64_t, i, worlds[i]->ID());
}

ADD_MANAGED_METHOD(World, Exists, bool, (uint64_t worldID))
{ return World::GetWorld(worldID) != nullptr; }

ADD_MANAGED_METHOD(World, Destroy, bool, (uint64_t worldID))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return false;

	// Unload from scenes if exists
	Systems::SceneSystem::UnloadScene(world);

	world->Destroy();
	delete world;
	return true;
}

ADD_MANAGED_METHOD(World, Create, uint64_t, (MonoString* nameRaw))
{
	char* name = mono_string_to_utf8(nameRaw);
	World* world = new World(name);
	mono_free(name);
	return world->ID();
}

ADD_MANAGED_METHOD(World, SetName, void, (uint64_t worldID, MonoString* nameRaw))
{
	char* name = mono_string_to_utf8(nameRaw);
	World* world = World::GetWorld(worldID);
	if (world)
		world->Name(name);
	mono_free(name);
}

ADD_MANAGED_METHOD(World, CreateEntity, uint64_t, (uint64_t worldID))
{
	World* world = World::GetWorld(worldID);
	return world ? world->CreateEntity().ID() : InvalidEntityID;
}

ADD_MANAGED_METHOD(World, CreateEntityID, uint64_t, (uint64_t worldID, uint64_t entityID))
{
	World* world = World::GetWorld(worldID);
	return world ? world->CreateEntity(entityID).ID() : InvalidEntityID;
}

ADD_MANAGED_METHOD(World, DestroyEntity, void, (uint64_t worldID, uint64_t entityID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		world->DestroyEntity(entityID);
}

ADD_MANAGED_METHOD(World, HasEntity, bool, (uint64_t worldID, uint64_t entityID))
{
	World* world = World::GetWorld(worldID);
	return world ? world->HasEntity(entityID) : false;
}

ADD_MANAGED_METHOD(World, GetEntities, MonoArray*, (uint64_t worldID))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	auto entities = world->Entities();
	MonoArray* entityIDs = mono_array_new(mono_domain_get(), mono_get_uint64_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(entityIDs, uint64_t, i, entities[i].ID());
	return entityIDs;
}

ADD_MANAGED_METHOD(World, GetComponents, MonoArray*, (uint64_t worldID, MonoReflectionType* componentType))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	vector<EntityID> entities = world->GetComponentManager()->GetEntities(_GetComponentType(componentType));
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint64_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(output, uint64_t, i, entities[i]);
	return output;
}

ADD_MANAGED_METHOD(World, GetComponentsMultiple, MonoArray*, (uint64_t worldID, MonoArray* inputTypes))
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
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint64_class(), entities.size());
	for (size_t i = 0; i < entities.size(); i++)
		mono_array_set(output, uint64_t, i, entities[i]);
	return output;
}
#pragma endregion

#pragma region Entity
ADD_MANAGED_METHOD(Entity, HasComponent, bool, (uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType))
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	return world->GetComponentManager()->Has(entityID, _GetComponentType(componentType));
}

ADD_MANAGED_METHOD(Entity, GetComponent, MonoObject*, (uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType));

ADD_MANAGED_METHOD(Entity, AddComponent, MonoObject*, (uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType, void** outHandle))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;

	MonoType* managedType = mono_reflection_type_get_type(componentType);
	size_t typeHash = Assembly::GetTypeHash(managedType);

	// Check for existing component of same type on entity
	if (world->GetComponentManager()->Has(entityID, typeHash))
	{
		Component* component = world->GetComponentManager()->Get(entityID, typeHash);
		*outHandle = component;
		return component->ManagedData.GetInstance();
	}

	// Get unmanaged->managed data
	Component* component = nullptr;
	Assembly::ManagedComponentData managedData = Assembly::GetManagedComponentData(typeHash);
	if (!managedData.AddFn)
		component = world->AddComponent(entityID, managedType);
	else
		component = managedData.AddFn(world, entityID);
	
	*outHandle = component;
	return component->ManagedData.GetInstance();
}

ADD_MANAGED_METHOD(Entity, RemoveComponent, bool, (uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType))
{
	World* world = World::GetWorld(worldID);
	if (!world || !world->HasEntity(entityID))
		return false;

	size_t type = _GetComponentType(componentType);
	Components::Component* component = (Components::Component*)world->GetComponentManager()->Get(entityID, type);
	/*
	if (component && component->ManagedData.IsValid())
	{
		// Call Disabled() and then Destroyed()
		MonoException* exception = nullptr;
		MonoObject* instance = component->ManagedData.GetInstance();
		ComponentMethodEnabled(instance, false, &exception);
		ComponentMethodDestroyed(instance, &exception);
	}
	*/

	return world->GetComponentManager()->Remove(entityID, type);
}

ADD_MANAGED_METHOD(Entity, GetComponent, MonoObject*, (uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType))
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

ADD_MANAGED_METHOD(Entity, GetComponents, MonoArray*, (uint64_t worldID, uint64_t entityID))
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;

	// Key: type hash
	// Value: component instance
	auto componentPairs = world->GetComponentManager()->Get(entityID);

	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_object_class(), componentPairs.size());
	for (int i = 0; i < componentPairs.size(); i++)
	{
		Components::Component* component = (Components::Component*)componentPairs[i].second;
		MonoObject* instance = component->ManagedData.GetInstance();
		mono_array_set(output, MonoObject*, i, instance);
	}

	return output;
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

SystemManager* GetSystemManager(uint64_t worldID)
{
	if (worldID == InvalidEntityID)
		return SystemManager::Global();

	World* world = World::GetWorld(worldID);
	return world ? world->GetSystemManager() : nullptr;
}

ADD_MANAGED_METHOD(World, HasSystem, bool, (uint64_t worldID, MonoReflectionType* systemType))
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return false;
	return systemManager->Has(GetSystemType(systemType));
}

ADD_MANAGED_METHOD(World, GetSystem, MonoObject*, (uint64_t worldID, MonoReflectionType* systemType))
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

ADD_MANAGED_METHOD(World, GetSystems, MonoArray*, (uint64_t worldID))
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return nullptr;

	vector<Systems::System*> systems = systemManager->All();
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_object_class(), systems.size());
	for (size_t i = 0; i < systems.size(); i++)
		mono_array_set(output, MonoObject*, i, systems[i]->ManagedData.GetInstance());
	return output;
}

ADD_MANAGED_METHOD(World, AddSystem, MonoObject*, (uint64_t worldID, MonoReflectionType* systemType))
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
	if (systemData.AddFn == nullptr)
		return systemManager->Add(managedType)->ManagedData.GetInstance();
	return systemData.AddFn(systemManager)->ManagedData.GetInstance();
}

ADD_MANAGED_METHOD(World, RemoveSystem, bool, (uint64_t worldID, MonoReflectionType* systemType))
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

ADD_MANAGED_METHOD(World, EnableSystem, void, (uint64_t worldID, MonoReflectionType* systemType, bool enable))
{
	SystemManager* systemManager = GetSystemManager(worldID);
	if (!systemManager)
		return;

	size_t type = GetSystemType(systemType);
	Systems::System* system = systemManager->Get(type);
	if (!system)
		return;

	system->Enable(enable);
	if (!system->ManagedData.IsValid())
		return;

	MonoException* exception = nullptr;
	MonoObject* instance = system->ManagedData.GetInstance();
	SystemMethodEnabled(instance, enable, &exception);
}

#pragma endregion
