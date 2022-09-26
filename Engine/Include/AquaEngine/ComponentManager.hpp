#pragma once
#include <vector>
#include <utility>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <AquaEngine/Entity.hpp>

namespace AquaEngine
{
	namespace Components { struct Component; }
	
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
			std::vector<Components::Component*> Instances;
			std::unordered_map<EntityID, unsigned int> EntityIndex;

			void Destroy();

			bool Has(EntityID entity);
			void Remove(EntityID entity);
			std::vector<EntityID> GetEntities();
			Components::Component* Get(EntityID entity);
			void Add(Components::Component* component, EntityID entity);

			template<typename T>
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

		unsigned int m_WorldID;

		std::unordered_map<std::type_index, ComponentData> m_ComponentArrays;
		std::unordered_map<EntityID, std::vector<std::type_index>> m_EntityComponents;

	public:
		AquaAPI ComponentManager(unsigned int worldID) : m_WorldID(worldID) { }

		/// <summary>
		/// Release all resources
		/// </summary>
		AquaAPI void Destroy();

		/// <summary>
		/// Create a component and add it to an entity
		/// </summary>
		/// <returns>Created component</returns>
		template<typename T>
		T* Add(EntityID id)
		{
			std::type_index type = typeid(T);
			if (m_ComponentArrays.find(type) == m_ComponentArrays.end())
				m_ComponentArrays.emplace(type, ComponentData());
			T* component = new T();
			m_ComponentArrays[type].Add(component, id);

			if (m_EntityComponents.find(id) == m_EntityComponents.end())
				m_EntityComponents.emplace(id, std::vector<std::type_index>());
			m_EntityComponents[id].push_back(type);
			
			return component;
		}

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

		/// <summary>
		/// Gets component from entity
		/// </summary>
		/// <returns>Component on entity, or nullptr if doesn't exist</returns>
		template<typename T>
		T* Get(EntityID id)
		{
			if (IsEmpty(id))
				return nullptr;
			std::type_index type = typeid(T);
			return m_ComponentArrays[type].Get<T>(id);
		}

		/// <summary>
		/// Gets all components from entity
		/// </summary>
		/// <returns>All components, tuple of type and their data</returns>
		AquaAPI std::vector<std::pair<std::type_index, void*>> Get(EntityID id);

		/// <summary>
		/// Gets all entities with component
		/// </summary>
		/// <returns>Vector of components and associated entities, or empty if doesn't exist</returns>
		template<typename T>
		std::vector<T*> Get()
		{
			std::type_index type = typeid(T);
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
			std::type_index type = typeid(T);
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
			std::type_index type = typeid(T);
			return m_ComponentArrays.find(type) == m_ComponentArrays.end() ?
				std::vector<EntityID>() : m_ComponentArrays[type].GetEntities();
		}

		AquaAPI bool IsEmpty(EntityID id);

		template<typename T>
		bool Has(EntityID id)
		{
			std::type_index type = typeid(T);
			return m_ComponentArrays.find(type) != m_ComponentArrays.end() && m_ComponentArrays[type].Has(id);
		}

		template<typename T1, typename T2>
		bool Has(EntityID id) { return Has<T1>(id) && Has<T2>(id); }

		template<typename T1, typename T2, typename T3>
		bool Has(EntityID id) { return Has<T1>(id) && Has<T2>(id) && Has<T3>(id); }

		template<typename T1, typename T2, typename T3, typename T4>
		bool Has(EntityID id) { return Has<T1>(id) && Has<T2>(id) && Has<T3>(id) && Has<T4>(id); }

		template<typename T>
		void Remove(EntityID id)
		{
			if (!Has<T>(id))
				return;
			std::type_index type = typeid(T);
			if (m_ComponentArrays.find(type) == m_ComponentArrays.end())
				return;
			
			for(unsigned int i = 0; i < (unsigned int)m_EntityComponents[id].size(); i++)
			{
				if(m_EntityComponents[id][i] == type)
				{
					m_EntityComponents[id].erase(m_EntityComponents[id].begin() + i);
					break;
				}
			}
			
			m_ComponentArrays[type].Remove(id);
		}

		AquaAPI void Clear(EntityID id);
	};
}
