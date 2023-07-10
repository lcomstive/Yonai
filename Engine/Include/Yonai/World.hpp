#pragma once
#include <set>
#include <memory>
#include <string>
#include <unordered_map>
#include <Yonai/API.hpp>
#include <Yonai/UUID.hpp>
#include <Yonai/SystemManager.hpp>
#include <Yonai/ComponentManager.hpp>

namespace Yonai
{
	// Forward declarations
	namespace Systems { class SceneSystem;  }
	namespace Components { struct Component; struct ScriptComponent; }

	class World
	{
	private:
		UUID m_ID;
		std::string m_Name;

		std::unique_ptr<SystemManager> m_SystemManager;
		std::unique_ptr<ComponentManager> m_ComponentManager;

		static std::unordered_map<UUID, World*> s_Worlds;

		YonaiAPI void SetupEntityComponent(EntityID id, Components::Component* component);

		// When this world is added or removed from active scenes
		void OnActiveStateChanged(bool isActive);

		// Called once per frame
		void Update();

		friend class Systems::SceneSystem;

	public:
		struct Entity
		{
		private:
			EntityID m_ID;
			World* m_World;

		public:
			YonaiAPI Entity(World* world = nullptr) : m_ID(), m_World(world) { }
			YonaiAPI Entity(EntityID id, World* world = nullptr) : m_ID(id), m_World(world) { }

			YonaiAPI EntityID ID();
			YonaiAPI Yonai::World* GetWorld();

			YonaiAPI void Destroy();

			/// <summary>
			/// Adds a managed (C#) script component to this entity
			/// </summary>
			/// <returns>Created instance of scripted component, or nullptr if invalid</returns>
			YonaiAPI Components::ScriptComponent* AddComponent(MonoType* managedType);

			/// <returns>Instance of managed component, or nullptr if not found</returns>
			YonaiAPI Components::ScriptComponent* GetComponent(MonoType* managedType);

			/// <returns>True if this entity has a managed component matching type</returns>
			YonaiAPI bool HasComponent(MonoType* managedType);

			/// <summary>
			/// Removes a managed (C#) script component from this entity
			/// </summary>
			YonaiAPI void RemoveComponent(MonoType* managedType);

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

			YonaiAPI bool HasComponents();

			template<typename T1, typename T2>
			bool HasComponents() { return m_World ? m_World->HasComponents<T1, T2>(m_ID) : nullptr; }

			template<typename T1, typename T2, typename T3>
			bool HasComponents() { return m_World ? m_World->HasComponents<T1, T2, T3>(m_ID) : nullptr; }

			template<typename T1, typename T2, typename T3, typename T4>
			bool HasComponents() { return m_World ? m_World->HasComponents<T1, T2, T3, T4>(m_ID) : nullptr; }

			YonaiAPI void ClearComponents();

			YonaiAPI bool IsValid();
		};
		
	private:
		std::unordered_map<EntityID, Entity> m_Entities;
			
	public:

		YonaiAPI World(std::string name = "World");

		YonaiAPI UUID& ID();
		YonaiAPI std::string& Name();
		YonaiAPI void Name(char* name);
		YonaiAPI void Name(std::string& name);

		YonaiAPI void Destroy();

#pragma region Entity
		YonaiAPI Entity CreateEntity();
		YonaiAPI Entity CreateEntity(EntityID id);
		YonaiAPI bool HasEntity(EntityID entity);
		YonaiAPI void DestroyEntity(EntityID entity);

		/// <summary>
		/// Gets an entity with matching ID, or creates entity if it does not exist
		/// </summary>
		YonaiAPI Entity GetEntity(EntityID entity);

		template<typename T>
		Entity CreateEntity()
		{
			Entity e = Entity(EntityID(), this);
			m_Entities.emplace(e.ID(), e);
			AddComponent<T>(e.ID());
			return e;
		}

		template<typename T1, typename T2>
		Entity CreateEntity()
		{
			Entity e = Entity(EntityID(), this);
			AddComponent<T1, T2>(e.ID());
			m_Entities.emplace(e.ID(), e);
			return e;
		}

		template<typename T1, typename T2, typename T3>
		Entity CreateEntity()
		{
			Entity e = Entity(EntityID(), this);
			AddComponent<T1, T2, T3>(e.ID());
			m_Entities.emplace(e.ID(), e);
			return e;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		Entity CreateEntity()
		{
			Entity e = Entity(EntityID(), this);
			AddComponent<T1, T2, T3, T4>(e.ID());
			m_Entities.emplace(e.ID(), e);
			return e;
		}

		YonaiAPI size_t EntityCount();

		YonaiAPI std::vector<Entity> Entities();

		template<typename T>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_ComponentManager->Entities<T>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_ComponentManager->Entities<T1, T2>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2, typename T3>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_ComponentManager->Entities<T1, T2, T3>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		std::vector<Entity> Entities()
		{
			std::vector<EntityID> IDs = m_ComponentManager->Entities<T1, T2, T3, T4>();
			std::vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}
#pragma endregion

#pragma region Components
		/// <summary>
		/// Gets an instance of a component
		/// </summary>
		void* GetComponent(EntityID entity, size_t type);

		/// <summary>
		/// Gets an instance of a component
		/// </summary>
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
		T* AddComponent(EntityID id, size_t type)
		{
			if (HasComponent<T>(id))
				return GetComponent<T>(id);

			T* component = m_ComponentManager->Add<T>(id, type);
			if (std::is_base_of<Components::Component, T>())
				SetupEntityComponent(id, (Components::Component*)component);
			return component;
		}

		/// <summary>
		/// Adds a managed (C#) script component to entity
		/// </summary>
		/// <returns>Created instance of scripted component, or nullptr if invalid</returns>
		YonaiAPI Components::ScriptComponent* AddComponent(EntityID entity, MonoType* managedType);
	
		/// <returns>Instance of managed component, or nullptr if not found</returns>
		YonaiAPI Components::ScriptComponent* GetComponent(EntityID entity, MonoType* managedType);
		
		/// <returns>True if entity has a managed component matching type</returns>
		YonaiAPI bool HasComponent(EntityID entity, MonoType* managedType);

		/// <summary>
		/// Removes a managed (C#) script component from entity
		/// </summary>
		YonaiAPI void RemoveComponent(EntityID entity, MonoType* managedType);

		template<typename T>
		T* AddComponent(EntityID entity)
		{ return AddComponent<T>(entity, typeid(T).hash_code()); }

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

		YonaiAPI bool HasComponents(EntityID entity);

		template<typename T1, typename T2>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2>(entity); }

		template<typename T1, typename T2, typename T3>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2, T3>(entity); }

		template<typename T1, typename T2, typename T3, typename T4>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2, T3, T4>(entity); }

		YonaiAPI void ClearComponents(EntityID entity);
#pragma endregion

#pragma region Getters
		YonaiAPI Yonai::SystemManager* GetSystemManager();
		YonaiAPI Yonai::ComponentManager* GetComponentManager();

		static std::vector<World*> GetWorlds();
		static World* GetWorld(UUID id);
		static bool Exists(UUID id);
#pragma endregion
	};

	typedef struct World::Entity Entity;
}