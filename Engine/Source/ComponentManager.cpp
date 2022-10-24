#include <AquaEngine/ComponentManager.hpp>
#include <AquaEngine/Components/Component.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Components;

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
{ return Has(id, type.hash_code()); }

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

#pragma region ComponentData
void ComponentManager::ComponentData::Destroy()
{
	for (size_t i = 0; i < Instances.size(); i++)
		delete Instances[i];
	Instances.clear();
	EntityIndex.clear();
}

void ComponentManager::ComponentData::Add(void* instance, EntityID entity)
{
	Instances.emplace_back(instance);
	EntityIndex.emplace(entity, (unsigned int)Instances.size() - 1);
}

void* ComponentManager::ComponentData::Get(EntityID entity) { return Has(entity) ? Instances[EntityIndex[entity]] : nullptr; }

bool ComponentManager::ComponentData::Has(EntityID entity) { return EntityIndex.find(entity) != EntityIndex.end(); }

void ComponentManager::ComponentData::Remove(EntityID entity)
{
	unsigned int instanceIndex = EntityIndex[entity];
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