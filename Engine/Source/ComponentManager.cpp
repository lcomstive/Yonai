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

vector<pair<type_index, void*>> ComponentManager::Get(EntityID id)
{
	std::vector<std::pair<std::type_index, void*>> components;

	if (m_EntityComponents.find(id) == m_EntityComponents.end())
		return components;

	for (auto& type : m_EntityComponents[id])
		components.emplace_back(type, m_ComponentArrays[type].Get(id));

	return components;
}

bool ComponentManager::IsEmpty(EntityID id)
{
	return m_EntityComponents.find(id) == m_EntityComponents.end() ||
			m_EntityComponents[id].size() == 0;
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

#pragma region ComponentData
void ComponentManager::ComponentData::Destroy()
{
	for (size_t i = 0; i < Instances.size(); i++)
		delete Instances[i];
	Instances.clear();
	EntityIndex.clear();
}

void ComponentManager::ComponentData::Add(Component* component, EntityID entity)
{
	Instances.emplace_back(component);
	EntityIndex.emplace(entity, (unsigned int)Instances.size() - 1);
}

Component* ComponentManager::ComponentData::Get(EntityID entity) { return Has(entity) ? Instances[EntityIndex[entity]] : nullptr; }

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