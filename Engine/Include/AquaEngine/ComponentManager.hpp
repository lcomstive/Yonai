#pragma once
#include <vector>
#include <utility>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Entity.hpp>
#include <AquaEngine/Scripting/ManagedData.hpp>

namespace AquaEngine
{	
	// Forward declaration
	class World;
	namespace Scripting { class ScriptEngine; }
	namespace Components { struct Component; struct ScriptComponent; }

	/// <summary>
	/// Handles many entities & their related component instances
	/// </summary>
	class ComponentManager
	{
	private:
		/// <summary>
		/// Stores component instance data and attached entities
		/// </summary>
		struct ComponentData
		{
			ComponentManager* Owner = nullptr;

			/// <summary>
			/// Hash of type that all instances share
			/// </summary>
			size_t TypeHash = 0;

			/// <summary>
			/// Array of all created instances
			/// </summary>
			std::vector<Components::Component*> Instances;

			/// <summary>
			/// Maps EntityID to index inside Instances
			/// </summary>
			std::unordered_map<EntityID, unsigned int> EntityIndex;

			/// <summary>
			/// Release all instances
			/// </summary>
			AquaAPI void Destroy();

			/// <returns>True if entity has an instance</returns>
			AquaAPI bool Has(EntityID entity);

			/// <returns>Instance of component attached to entity matching ID, or nullptr if not found</returns>
			AquaAPI Components::Component* Get(EntityID entity);

			/// <summary>
			/// Removes any found instance of component on entity
			/// </summary>
			AquaAPI void Remove(EntityID entity);

			/// <summary>
			/// Gets all entities with this component type
			/// </summary>
			AquaAPI std::vector<EntityID> GetEntities();

			/// <summary>
			/// Adds a new instance, attaching to entity
			/// </summary>
			AquaAPI void Add(Components::Component* instance, EntityID entity);

			template<typename T>
			/// <returns>Instance of component attached to entity matching ID, or nullptr if not found</returns>
			T* Get(EntityID entity) { return (T*)(Has(entity) ? Instances[EntityIndex[entity]] : nullptr); }

			template<typename T>
			std::vector<T*> Get()
			{
				std::vector<T*> components;
				components.reserve(Instances.size());
				for (const auto& pair : EntityIndex)
					components.push_back((T*)Instances[pair.second]);
				return components;
			}
		};

		AquaAPI unsigned int m_WorldID;

		bool m_WorldIsActive = false;
		std::unordered_map<size_t, ComponentData> m_ComponentArrays;
		std::unordered_map<EntityID, std::vector<size_t>> m_EntityComponents;
		
		void OnWorldActiveStateChanged(bool isActive);
		AquaEngine::Scripting::ManagedData CreateManagedInstance(size_t typeHash, unsigned int entityID);

		/// <summary>
		/// Releases all 'Component::ManagedInstance's,
		/// so the next time they are accessed they are regenerated
		/// </summary>
		void InvalidateAllManagedInstances();

		void CallUpdateFn();

		friend class World;
		friend class Scripting::ScriptEngine;

	public:
		AquaAPI ComponentManager(unsigned int worldID);

		/// <summary>
		/// Release all resources
		/// </summary>
		AquaAPI void Destroy();

		/// <summary>
		/// Create a component and add it to an entity
		/// </summary>
		/// <returns>Created component</returns>
		template<typename T>
		T* Add(EntityID id, size_t type)
		{
			if (!std::is_base_of<Components::Component, T>())
			{
				spdlog::warn("Cannot add component of type '{}' because it does not derive from AquaEngine::Components::Component",
					typeid(T).name());
				return nullptr;
			}

			T* component = new T();
			if (m_ComponentArrays.find(type) == m_ComponentArrays.end())
				m_ComponentArrays.emplace(type, ComponentData { this, type });
			m_ComponentArrays[type].Add((Components::Component*)component, id);

			if (m_EntityComponents.find(id) == m_EntityComponents.end())
				m_EntityComponents.emplace(id, std::vector<size_t>());
			m_EntityComponents[id].push_back(type);

			return component;
		}

		AquaAPI Components::ScriptComponent* Add(EntityID id, MonoType* managedType);

		/// <summary>
		/// Create a component and add it to an entity
		/// </summary>
		/// <returns>Created component</returns>
		template<typename T>
		T* Add(EntityID id) { return Add<T>(id, typeid(T).hash_code()); }

		/// <summary>
		/// Creates components and adds them to an entity
		/// </summary>
		template<typename T1, typename T2>
		void Add(EntityID id)
		{
			Add<T1>(id);
			Add<T2>(id);
		}

		/// <summary>
		/// Creates components and adds them to an entity
		/// </summary>
		template<typename T1, typename T2, typename T3>
		void Add(EntityID id)
		{
			Add<T1>(id);
			Add<T2>(id);
			Add<T3>(id);
		}

		template<typename T1, typename T2, typename T3, typename T4>
		/// <summary>
		/// Creates components and adds them to an entity
		/// </summary>
		void Add(EntityID id)
		{
			Add<T1>(id);
			Add<T2>(id);
			Add<T3>(id);
			Add<T4>(id);
		}

		Components::Component* Get(EntityID id, size_t type);

		std::vector<EntityID> GetEntities(size_t type);
		std::vector<EntityID> GetEntities(std::vector<size_t> types);

		/// <summary>
		/// Gets component from entity
		/// </summary>
		/// <returns>Component on entity, or nullptr if doesn't exist</returns>
		template<typename T>
		T* Get(EntityID id)
		{
			if (IsEmpty(id))
				return nullptr;
			size_t type = typeid(T).hash_code();
			return m_ComponentArrays[type].Get<T>(id);
		}

		/// <summary>
		/// Gets all components from entity
		/// </summary>
		/// <returns>All components, tuple of type and their data</returns>
		AquaAPI std::vector<std::pair<size_t, void*>> Get(EntityID id);

		/// <summary>
		/// Gets all entities with component
		/// </summary>
		/// <returns>Vector of components and associated entities, or empty if doesn't exist</returns>
		template<typename T>
		std::vector<T*> Get()
		{
			size_t type = typeid(T).hash_code();
			return m_ComponentArrays.find(type) == m_ComponentArrays.end() ?
				std::vector<T*>() : m_ComponentArrays[type].Get<T>();
		}

		/// <summary>
		/// Gets components of type from entities
		/// </summary>
		/// <returns>Vector of components on entities</returns>
		template<typename T>
		std::vector<T*> Get(EntityID entities[], unsigned int entityCount)
		{
			size_t type = typeid(T).hash_code();
			std::vector<T*> components;
			for (unsigned int i = 0; i < entityCount; i++)
			{
				T* component = m_ComponentArrays[type].Get<T>(entities[i]);
				if (component)
					components.push_back(component);
			}
			return components;
		}

		/// <summary>
		/// Gets components of type from entities
		/// </summary>
		/// <returns>Vector of components on entities</returns>
		template<typename T>
		std::vector<T*> Get(std::vector<EntityID> entities) { return Get<T>(entities.data(), (unsigned int)entities.size()); }

		/// <summary>
		/// Gets all entities with desired components
		/// </summary>
		template<typename T1, typename T2>
		std::vector<std::pair<T1*, T2*>> Get()
		{
			std::vector<T1*> type1 = Get<T1>();
			std::vector<T2*> type2 = Get<T2>();

			std::vector<std::pair<T1*, T2*>> components;

			for (T1* t1 : type1)
			{
				for (T2* t2 : type2)
				{
					if (t1->Entity.ID() == t2->Entity.ID())
						components.emplace_back(std::make_pair(t1, t2));
				}
			}

			return components;
		}

		/// <summary>
		/// Gets all entities with component of type
		/// </summary>
		template<typename T>
		std::vector<EntityID> Entities()
		{
			size_t type = typeid(T).hash_code();
			return m_ComponentArrays.find(type) == m_ComponentArrays.end() ?
				std::vector<EntityID>() : m_ComponentArrays[type].GetEntities();
		}

		AquaAPI bool IsEmpty(EntityID& id);

		AquaAPI bool Has(EntityID& id, size_t type);
		AquaAPI bool Has(EntityID& id, std::type_info& type);

		template<typename T>
		bool Has(EntityID& id) { return Has(id, typeid(T).hash_code()); }

		template<typename T1, typename T2>
		bool Has(EntityID& id) { return Has<T1>(id) && Has<T2>(id); }

		template<typename T1, typename T2, typename T3>
		bool Has(EntityID& id) { return Has<T1>(id) && Has<T2>(id) && Has<T3>(id); }

		template<typename T1, typename T2, typename T3, typename T4>
		bool Has(EntityID& id) { return Has<T1>(id) && Has<T2>(id) && Has<T3>(id) && Has<T4>(id); }

		AquaAPI bool Remove(EntityID& id, size_t type);

		template<typename T>
		bool Remove(EntityID& id) { return Remove(id, typeid(T).hash_code()); }

		AquaAPI void Clear(EntityID id);
	};
}
