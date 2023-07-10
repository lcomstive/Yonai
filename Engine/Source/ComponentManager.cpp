#include <Yonai/ComponentManager.hpp>
#include <Yonai/Components/Component.hpp>

#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Scripting/UnmanagedThunks.hpp>
#include <Yonai/Components/ScriptComponent.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Scripting;
using namespace Yonai::Components;

extern ComponentMethodInitialiseFn ComponentMethodInitialise;

ComponentManager::ComponentManager(UUID worldID) : m_WorldID(worldID) { }

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
	{
		Component* component = m_ComponentArrays[type].Get(id);
			components.emplace_back(type, component);
	}

	return components;
}

ScriptComponent* ComponentManager::Add(EntityID id, MonoType* managedType)
{
	size_t typeHash = Scripting::Assembly::GetTypeHash(managedType);
	ScriptComponent* component = Add<ScriptComponent>(id, typeHash);

	// Set managed type in component
	component->Type = managedType;

	return component;
}

bool ComponentManager::IsEmpty(EntityID id)
{
	return m_EntityComponents.find(id) == m_EntityComponents.end() ||
		m_EntityComponents[id].size() == 0;
}

bool ComponentManager::Has(EntityID id, size_t type)
{
	return m_ComponentArrays.find(type) != m_ComponentArrays.end() && m_ComponentArrays[type].Has(id);
}

bool ComponentManager::Has(EntityID id, type_info& type)
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

bool ComponentManager::Remove(EntityID id, size_t type)
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
	for (auto componentPair : m_ComponentArrays)
	{
		auto entities = componentPair.second.GetEntities();
		for (auto entityPair : componentPair.second.EntityIndex)
		{
			Component* instance = componentPair.second.Instances[entityPair.second];
			if (!instance->ManagedData.IsValid())
				instance->ManagedData = CreateManagedInstance(componentPair.first, entityPair.first);
		}
	}
}

Component* ComponentManager::Get(EntityID id, size_t type)
{ return IsEmpty(id) ? nullptr : m_ComponentArrays[type].Get(id); }

vector<EntityID> ComponentManager::GetEntities(size_t type)
{
	return m_ComponentArrays.find(type) == m_ComponentArrays.end() ? vector<EntityID>() : m_ComponentArrays[type].GetEntities();
}

vector<EntityID> ComponentManager::GetEntities(vector<size_t> types)
{
	vector<EntityID> output;
	
	// Store all possible entity IDs in output
	output.reserve(m_EntityComponents.size());
	for (auto pair : m_EntityComponents)
		output.emplace_back(pair.first);
	sort(output.begin(), output.end());

	// Check for intersection of each type against 'output' entity IDs.
	// This removes any IDs where that entity does not have any of the types
	for (size_t type : types)
	{
		vector<EntityID> entities = GetEntities(type);
		sort(entities.begin(), entities.end());

		// Calculate intersection, and resize using returned iterator
		output.resize(set_intersection(
			entities.begin(), entities.end(),
			output.begin(), output.end(),
			output.begin()
		) - output.begin());
	}
	return output;
}

void ComponentManager::InvalidateAllManagedInstances()
{
	for (auto componentPair : m_ComponentArrays)
	{
		for (Component* instance : componentPair.second.Instances)
		{
			if (!instance->ManagedData.IsValid())
				continue;

			mono_gchandle_free(instance->ManagedData.GCHandle);
			instance->ManagedData = {};
		}
	}
}

Yonai::Scripting::ManagedData ComponentManager::CreateManagedInstance(size_t typeHash, UUID entityID)
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

	return {
		managedData.AddFn == nullptr,
		mono_gchandle_new(instance, false),
		managedType
	};
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
	if (!instance->ManagedData.IsValid())
		return;
}

Component* ComponentManager::ComponentData::Get(EntityID entity)
{
	if (!Has(entity))
		return nullptr;
	Component* instance = Instances[EntityIndex[entity]];
	if (!instance->ManagedData.IsValid())
		instance->ManagedData = Owner->CreateManagedInstance(TypeHash, entity);
	return instance;
}

bool ComponentManager::ComponentData::Has(EntityID entity) { return EntityIndex.find(entity) != EntityIndex.end(); }

void ComponentManager::ComponentData::Remove(EntityID entity)
{
	unsigned int instanceIndex = EntityIndex[entity];

	// Free managed memory
	Component* instance = Instances[instanceIndex];
	if(instance->ManagedData.IsValid())
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
	int i = 0;
	for (const auto& pair : EntityIndex)
			entities[i++] = pair.first;
	return entities;
}
#pragma endregion