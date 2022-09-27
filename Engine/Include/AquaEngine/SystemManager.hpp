#pragma once
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <AquaEngine/Systems/System.hpp>

namespace AquaEngine
{
	class World;

	class SystemManager
	{
		World* m_Owner;

		static SystemManager* s_Global;

	public:
		AquaAPI SystemManager(World* owner);

		AquaAPI void Init();
		AquaAPI void Destroy();
		
		AquaAPI World* GetWorld();
		AquaAPI static SystemManager* Global();

		template<typename T>
		T* Add()
		{
			if (!std::is_base_of<Systems::System, T>())
				return nullptr;

			T* system = Get<T>();
			if (system) // Check if system already added
				return system;
			system = new T();
			system->m_Owner = this;
			m_Systems.emplace(typeid(T), system);
			return system;
		}

		template<typename T>
		void Remove()
		{
			if (!Has<T>())
				return;
			std::type_index type = typeid(T);
			delete m_Systems[type];
			m_Systems.erase(type);
		}

		template<typename T>
		bool Has() { return m_Systems.find(typeid(T)) != m_Systems.end(); }

		template<typename T>
		/// <returns>Pointer to system, or nullptr if not available</returns>
		T* Get()
		{
			if (!Has<T>())
				return nullptr;
			return (T*)m_Systems[typeid(T)];
		}

		AquaAPI void Draw();
		AquaAPI void Update();

		AquaAPI std::vector<Systems::System*> All();

	private:
		AquaAPI std::unordered_map<std::type_index, Systems::System*> m_Systems = {};
	};
}
