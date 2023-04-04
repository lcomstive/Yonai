#pragma once
#include <map>
#include <vector>
#include <AquaEngine/Entity.hpp>
#include <AquaEngine/ComponentManager.hpp>

namespace AquaEngine
{
	/// <summary>
	/// Handles creation and deletion of entities
	/// </summary>
	class EntityManager
	{
	public:
		EntityManager(ComponentManager* componentManager, unsigned int worldID);

		AquaAPI void Destroy();

		AquaAPI EntityID Create();
		AquaAPI bool Insert(EntityID id);
		AquaAPI void Prepare(unsigned int count = m_DefaultChunkSize);

		template<typename T>
		EntityID Create()
		{			
			EntityID id = Create();
			m_ComponentManager->Add<T>(id);
			return id;
		}

		template<typename T1, typename T2>
		EntityID Create()
		{
			EntityID id = Create();
			m_ComponentManager->Add<T1, T2>(id);
			return id;
		}

		template<typename T1, typename T2, typename T3>
		EntityID Create()
		{
			EntityID id = Create();
			m_ComponentManager->Add<T1, T2, T3>(id);
			return id;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		EntityID Create()
		{
			EntityID id = Create();
			m_ComponentManager->Add<T1, T2, T3, T4>(id);
			return id;
		}

		AquaAPI void Destroy(EntityID id);

		AquaAPI unsigned int EntityCount() { return m_EntityCount - m_AvailableEntityCount; }

		AquaAPI std::vector<EntityID> Entities();

		template<typename T>
		std::vector<EntityID> Entities()
		{
			return m_ComponentManager->Entities<T>();
		}

		template<typename T1, typename T2>
		std::vector<EntityID> Entities()
		{
			std::vector<EntityID> entities;
			for (EntityID i = 0; i < m_EntityCount; i++)
			{
				if (m_Entities[i]) // if entity available (not being used)
					continue;
				if (m_ComponentManager->Has<T1, T2>(i))
					entities.emplace_back(i);
			}
			return entities;
		}

		template<typename T1, typename T2, typename T3>
		std::vector<EntityID> Entities()
		{
			std::vector<EntityID> entities;
			for (EntityID i = 0; i < m_EntityCount; i++)
			{
				if (m_Entities[i]) // if entity available (not being used)
					continue;
				if (m_ComponentManager->Has<T1, T2, T3>(i))
					entities.emplace_back(i);
			}
			return entities;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		std::vector<EntityID> Entities()
		{
			std::vector<EntityID> entities;
			for (EntityID i = 0; i < m_EntityCount; i++)
			{
				if (m_Entities[i]) // if entity available (not being used)
					continue;
				if (m_ComponentManager->Has<T1, T2, T3, T4>(i))
					entities.emplace_back(i);
			}
			return entities;
		}

	private:
		unsigned int m_WorldID;
		ComponentManager* m_ComponentManager;

		static const unsigned int m_DefaultChunkSize = 512;
		bool* m_Entities;
		unsigned int m_EntityCount = 0;
		unsigned int m_AvailableEntityCount = 0;

		void AddChunk(unsigned int chunkSize = m_DefaultChunkSize);
		void RemoveChunk();
		EntityID NextAvailableEntityID();
	};
}
