#include <AquaEngine/ComponentManager.hpp>

using namespace std;
using namespace AquaEngine;

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