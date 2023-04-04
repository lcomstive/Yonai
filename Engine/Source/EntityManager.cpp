#include <cstring> // memcpy
#include <iostream>
#include <AquaEngine/World.hpp>
#include <AquaEngine/EntityManager.hpp>

using namespace std;
using namespace AquaEngine;

EntityManager::EntityManager(ComponentManager* componentManager, unsigned int worldID) : m_ComponentManager(componentManager), m_WorldID(worldID), m_Entities(nullptr) { }

void EntityManager::Destroy() { delete[] m_Entities; }

EntityID EntityManager::Create()
{
	EntityID id = NextAvailableEntityID();
	m_Entities[id] = false;
	m_AvailableEntityCount--;
	
	return id;
}

bool EntityManager::Insert(EntityID id)
{
	if (!m_Entities[id])
		return false;

	m_Entities[id] = false;
	m_AvailableEntityCount--;
	return true;
}

void EntityManager::Destroy(EntityID id)
{
	m_ComponentManager->Clear(id);
	m_Entities[id] = true;
	m_AvailableEntityCount++;
}

vector<EntityID> EntityManager::Entities()
{
	vector<EntityID> entities;
	for (EntityID i = 0; i < m_EntityCount; i++)
		if (!m_Entities[i])
			entities.emplace_back(i);
	return entities;
}

void EntityManager::Prepare(unsigned int count)
{
	EntityID id = 0;
	unsigned int spareIDs = 0;
	for (id = 0; id < m_EntityCount; id++)
	{
		if (!m_Entities[id])
		{
			spareIDs++;
			if (spareIDs >= count)
				return;
		}
	}

	AddChunk(count - spareIDs);
}

EntityID EntityManager::NextAvailableEntityID()
{
	EntityID id = 0;
	for (id = 0; id < m_EntityCount; id++)
		if (m_Entities[id])
			return id;
	AddChunk();
	return id;
}

void EntityManager::AddChunk(unsigned int chunkSize)
{
	bool* oldEntities = m_Entities;
	m_Entities = new bool[m_EntityCount + chunkSize];
	memcpy(m_Entities, oldEntities, m_EntityCount);
	delete[] oldEntities;

	for (unsigned int i = m_EntityCount; i < m_EntityCount + chunkSize; i++)
		m_Entities[i] = true;

	m_EntityCount += chunkSize;
	m_AvailableEntityCount += chunkSize;
}

void EntityManager::RemoveChunk()
{
	// TODO: Remove trailing available entities
}