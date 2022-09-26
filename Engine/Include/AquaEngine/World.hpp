#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <AquaEngine/API.hpp>
#include <AquaEngine/EntityManager.hpp>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/ComponentManager.hpp>

namespace AquaEngine
{
	namespace Components { struct Component; }

	class World
	{
	private:
		unsigned int m_ID;
		std::string m_Name;
		std::unique_ptr<EntityManager> m_EntityManager;
		std::unique_ptr<SystemManager> m_SystemManager;
		std::unique_ptr<ComponentManager> m_ComponentManager;

		static std::unordered_map<unsigned int, World*> s_Worlds;

		AquaAPI void SetupEntityComponent(EntityID id, Components::Component* component);

	public:
		struct Entity
		{
		private:
			EntityID m_ID;
			World* m_World;

		public:
			AquaAPI Entity(EntityID entity = 0, World* world = nullptr) : m_ID(entity), m_World(world) { }

			AquaAPI EntityID ID();
			AquaAPI AquaEngine::World* GetWorld();

			AquaAPI void Destroy();

			template<typename T>
			T* AddComponent() { return m_World ? m_World->AddComponent<T>(m_ID) : nullptr; }

			template<typename T1, typename T2>
			void AddComponent() { if (m_World)m_World->AddComponent<T1, T2>(m_ID); }

			template<typename T1, typename T2, typename T3>
			void AddComponent() { if (m_World) m_World->AddComponent<T1, T2, T3>(m_ID); }

			template<typename T1, typename T2, typename T3, typename T4>
			void AddComponent() { if (m_World) m_World->AddComponent<T1, T2, T3>(m_ID); }

			template<typename T>
			T* GetComponent() { return m_World ? m_World->GetComponent<T>(m_ID) : nullptr; }

			template<typename T>
			void RemoveComponent() { if (m_World) m_World->RemoveComponent<T>(m_ID); }

			template<typename T>
			bool HasComponent() { return m_World ? m_World->HasComponent<T>(m_ID) : false; }

			AquaAPI bool HasComponents();

			template<typename T1, typename T2>
			bool HasComponents() { return m_World ? m_World->HasComponents<T1, T2>(m_ID) : nullptr; }

			template<typename T1, typename T2, typename T3>
			bool HasComponents() { return m_World ? m_World->HasComponents<T1, T2, T3>(m_ID) : nullptr; }

			template<typename T1, typename T2, typename T3, typename T4>
			bool HasComponents() { return m_World ? m_World->HasComponents<T1, T2, T3, T4>(m_ID) : nullptr; }

			AquaAPI void ClearComponents();

			AquaAPI bool IsValid();
		};

		AquaAPI World(std::string name = "World");

		AquaAPI unsigned int ID();
		AquaAPI std::string& Name();
		AquaAPI void Name(std::string& name);

		AquaAPI void Destroy();

		/// ENTITY ///
		AquaAPI void DestroyEntity(EntityID entity);
		AquaAPI void PrepareEntities(unsigned int count);
		AquaAPI Entity CreateEntity();
		AquaAPI Entity GetEntity(EntityID entity);

		template<typename T>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create(), this);
			m_Entities.emplace(e.ID(), e);
			AddComponent<T>(e.ID());
			return e;
		}

		template<typename T1, typename T2>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create(), this);
			AddComponent<T1, T2>(e.ID());
			m_Entities.emplace(e.ID(), e);
			return e;
		}

		template<typename T1, typename T2, typename T3>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create(), this);
			AddComponent<T1, T2, T3>(e.ID());
			m_Entities.emplace(e.ID(), e);
			return e;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create(), this);
			AddComponent<T1, T2, T3, T4>(e.ID());
			m_Entities.emplace(e.ID(), e);
			return e;
		}

		AquaAPI unsigned int EntityCount();

		AquaAPI std::vector<Entity> Entities();

		template<typename T>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_EntityManager->Entities<T>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_EntityManager->Entities<T1, T2>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2, typename T3>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_EntityManager->Entities<T1, T2, T3>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_EntityManager->Entities<T1, T2, T3, T4>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		/// COMPONENT ///
		template<typename T>
		T* GetComponent(EntityID entity) { return m_ComponentManager->Get<T>(entity); }

		/// <summary>
		/// Gets all entities with component
		/// </summary>
		/// <returns>Map of components and associated entities, or empty if doesn't exist</returns>
		template<typename T>
		std::vector<T*> GetComponents() { return m_ComponentManager->Get<T>(); }

		template<typename T1, typename T2>
		std::vector<std::pair<T1*, T2*>> GetComponents() { return m_ComponentManager->Get<T1, T2>(); }

		template<typename T>
		T* AddComponent(EntityID entity)
		{
			if (HasComponent<T>(entity))
				return GetComponent<T>(entity);

			T* component = m_ComponentManager->Add<T>(entity);
			if (std::is_base_of<Components::Component, T>())
				SetupEntityComponent(entity, (Components::Component*)component);
			return component;
		}

		template<typename T1, typename T2>
		void AddComponent(EntityID entity)
		{
			AddComponent<T1>(entity);
			AddComponent<T2>(entity);
		}

		template<typename T1, typename T2, typename T3>
		void AddComponent(EntityID entity)
		{
			AddComponent<T1>(entity);
			AddComponent<T2>(entity);
			AddComponent<T3>(entity);
		}

		template<typename T1, typename T2, typename T3, typename T4>
		void AddComponent(EntityID entity)
		{
			AddComponent<T1>(entity);
			AddComponent<T2>(entity);
			AddComponent<T3>(entity);
			AddComponent<T4>(entity);
		}

		template<typename T>
		void RemoveComponent(EntityID entity) { m_ComponentManager->Remove<T>(entity); }

		template<typename T>
		bool HasComponent(EntityID entity) { return m_ComponentManager->Has<T>(entity); }

		AquaAPI bool HasComponents(EntityID entity);

		template<typename T1, typename T2>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2>(entity); }

		template<typename T1, typename T2, typename T3>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2, T3>(entity); }

		template<typename T1, typename T2, typename T3, typename T4>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2, T3, T4>(entity); }

		AquaAPI void ClearComponents(EntityID entity);

		/// Getters ///
		AquaAPI AquaEngine::SystemManager* GetSystemManager();
		AquaAPI AquaEngine::EntityManager* GetEntityManager();
		AquaAPI AquaEngine::ComponentManager* GetComponentManager();

		static World* GetWorld(unsigned int id) { return s_Worlds.find(id) == s_Worlds.end() ? nullptr : s_Worlds[id]; }

	private:
		std::unordered_map<EntityID, Entity> m_Entities;
	};

	typedef struct World::Entity Entity;
}
