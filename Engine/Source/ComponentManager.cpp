#include <AquaEngine/ComponentManager.hpp>
#include <AquaEngine/Components/Component.hpp>

#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>
#include <AquaEngine/Components/ScriptComponent.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

extern EmptyMethodFn ComponentMethodStart;
extern EmptyMethodFn ComponentMethodUpdate;
extern EmptyMethodFn ComponentMethodDestroyed;
extern ComponentMethodEnabledFn ComponentMethodEnabled;
extern ComponentMethodInitialiseFn ComponentMethodInitialise;

ComponentManager::ComponentManager(unsigned int worldID) : m_WorldID(worldID) { }

void ComponentManager::Destroy()
{
	for (auto& iterator : m_ComponentArrays)
		iterator.second.Destroy();
}

vector<pair<size_t, void*>> ComponentManager::Get(EntityID id)
{
	std::vector<std::pair<size_t, void*>> components;

	if (m_EntityComponents.find(id) == m_EntityComponents.end())
		return components;

	for (auto& type : m_EntityComponents[id])
		components.emplace_back(type, m_ComponentArrays[type].Get(id));

	return components;
}

ScriptComponent* ComponentManager::Add(EntityID id, MonoType* managedType)
{
	ScriptComponent* component = new ScriptComponent();
	size_t typeHash = Scripting::Assembly::GetTypeHash(managedType);

	// TODO: Check if component already exists on entity, and return existing component

	if (m_ComponentArrays.find(typeHash) == m_ComponentArrays.end())
		m_ComponentArrays.emplace(typeHash, ComponentData{ this, typeHash });
	m_ComponentArrays[typeHash].Add(component, id);

	if (m_EntityComponents.find(id) == m_EntityComponents.end())
		m_EntityComponents.emplace(id, std::vector<size_t>());
	m_EntityComponents[id].push_back(typeHash);

	// Set managed type in component
	component->Type = managedType;

	return component;
}

bool ComponentManager::IsEmpty(EntityID& id)
{
	return m_EntityComponents.find(id) == m_EntityComponents.end() ||
		m_EntityComponents[id].size() == 0;
}

bool ComponentManager::Has(EntityID& id, size_t type)
{
	return m_ComponentArrays.find(type) != m_ComponentArrays.end() && m_ComponentArrays[type].Has(id);
}

bool ComponentManager::Has(EntityID& id, type_info& type)
{
	return Has(id, type.hash_code());
}

void ComponentManager::Clear(EntityID id)
{
	if (IsEmpty(id))
		return;

	for (const auto& type : m_EntityComponents[id])
	{
		ComponentData* componentArray = &m_ComponentArrays[type];
		m_ComponentArrays[type].Remove(id);
	}
	m_EntityComponents[id].clear();
}

bool ComponentManager::Remove(EntityID& id, size_t type)
{
	if (!Has(id, type) || m_ComponentArrays.find(type) == m_ComponentArrays.end())
		return false;

	for (size_t i = 0; i < m_EntityComponents[id].size(); i++)
	{
		if (m_EntityComponents[id][i] == type)
		{
			m_EntityComponents[id].erase(m_EntityComponents[id].begin() + i);
			break;
		}
	}

	m_ComponentArrays[type].Remove(id);
	return true;
}

void ComponentManager::OnWorldActiveStateChanged(bool isActive)
{
	m_WorldIsActive = isActive;

	MonoException* exception = nullptr;
	// Call all components' Enabled/Disabled and Start functions
	for (auto componentPair : m_ComponentArrays)
	{
		auto entities = componentPair.second.GetEntities();
		for (auto entityPair : componentPair.second.EntityIndex)
		{
			Component* instance = componentPair.second.Instances[entityPair.second];
			if (!instance->ManagedData.Instance)
				instance->ManagedData = CreateManagedInstance(componentPair.first, entityPair.first);
			if (!instance->ManagedData.Instance)
				continue;

			ComponentMethodEnabled(instance->ManagedData.Instance, m_WorldIsActive, &exception);
			if (m_WorldIsActive)
				ComponentMethodStart(instance->ManagedData.Instance, &exception);
			else
				ComponentMethodDestroyed(instance->ManagedData.Instance, &exception);
		}
	}
}

void ComponentManager::InvalidateAllManagedInstances()
{
	for (auto componentPair : m_ComponentArrays)
	{
		for (Component* instance : componentPair.second.Instances)
		{
			mono_gchandle_free(instance->ManagedData.GCHandle);
			instance->ManagedData = {};
		}
	}
}

AquaEngine::Scripting::ManagedData ComponentManager::CreateManagedInstance(size_t typeHash, unsigned int entityID)
{
	MonoType* managedType = ScriptEngine::GetTypeFromHash(typeHash);
	Assembly::ManagedComponentData managedData = ScriptEngine::GetCoreAssembly()->GetManagedComponentData(typeHash);
	if (!managedType)
	{
		if (managedData.AddFn)
			return managedData.AddFn(World::GetWorld(m_WorldID), entityID)->ManagedData;
		return {};
	}

	// Create managed (C#) instance
	MonoDomain* domain = mono_domain_get();
	MonoObject* instance = mono_object_new(mono_domain_get(), mono_class_from_mono_type(managedType));

	// Initialise component
	MonoException* exception = nullptr;
	ComponentMethodInitialise(instance, m_WorldID, entityID, &exception);

	// Call constructor
	mono_runtime_object_init(instance);

	auto componentData = ScriptEngine::GetCoreAssembly()->GetManagedComponentData(typeHash);
	return {
		instance,
		managedData.AddFn == nullptr,
		mono_gchandle_new(instance, false),
		managedType
	};
}

void ComponentManager::CallUpdateFn()
{
	if (!ScriptEngine::IsLoaded())
		return;

	// Make local copy incase m_ComponentArrays gets modified
	auto componentArraysCopy = m_ComponentArrays;

	// Call all components' Update function
	for (auto componentPair : componentArraysCopy)
	{
		for (size_t i = 0; i < componentPair.second.Instances.size(); i++)
		{
			Component* instance = componentPair.second.Instances[i];

			if (!instance->Entity.GetWorld())
				continue; // Invalid entity, world is not set?

			if (instance->ManagedData.Instance && instance->ManagedData.ShouldSendMessages)
			{
				MonoException* exception = nullptr;
				ComponentMethodUpdate(instance->ManagedData.Instance, &exception);
			}
		}
	}
}

#pragma region ComponentData
void ComponentManager::ComponentData::Destroy()
{
	for (size_t i = 0; i < Instances.size(); i++)
		delete Instances[i];
	Instances.clear();
	EntityIndex.clear();
}

void ComponentManager::ComponentData::Add(Component* instance, EntityID entity)
{
	Instances.emplace_back(instance);
	EntityIndex.emplace(entity, (unsigned int)Instances.size() - 1);

	// Scripting
	if (!ScriptEngine::IsLoaded())
		return;

	// Create managed (C#) instance
	instance->ManagedData = Owner->CreateManagedInstance(TypeHash, entity);
	if (!instance->ManagedData.Instance)
		return;

	// If world is active, call Enabled and then Start
	MonoException* exception = nullptr;
	if (Owner->m_WorldIsActive)
	{
		ComponentMethodEnabled(instance->ManagedData.Instance, true, &exception);
		ComponentMethodStart(instance->ManagedData.Instance, &exception);
	}
}

Component* ComponentManager::ComponentData::Get(EntityID entity)
{
	if (!Has(entity))
		return nullptr;
	Component* instance = Instances[EntityIndex[entity]];
	if (!instance->ManagedData.Instance)
		instance->ManagedData = Owner->CreateManagedInstance(TypeHash, entity);
	return instance;
}

bool ComponentManager::ComponentData::Has(EntityID entity) { return EntityIndex.find(entity) != EntityIndex.end(); }

void ComponentManager::ComponentData::Remove(EntityID entity)
{
	unsigned int instanceIndex = EntityIndex[entity];

	// Free managed memory
	Component* instance = Instances[instanceIndex];
	if(instance->ManagedData.Instance)
		mono_gchandle_free(instance->ManagedData.GCHandle);
	
	// Free unmanaged memory
	delete Instances[instanceIndex];

	Instances.erase(Instances.begin() + instanceIndex);

	EntityIndex.erase(entity);

	// Shift all indexes down
	for (const auto& pair : EntityIndex)
	{
		if (pair.second >= instanceIndex)
			EntityIndex[pair.first]--;
	}
}

std::vector<EntityID> ComponentManager::ComponentData::GetEntities()
{
	std::vector<EntityID> entities;
	entities.resize(EntityIndex.size());
	unsigned int i = 0;
	for (const auto& pair : EntityIndex)
		entities[i++] = pair.first;
	return entities;
}
#pragma endregion