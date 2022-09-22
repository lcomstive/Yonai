#pragma once
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <AquaEngine/Systems/System.hpp>

namespace AquaEngine
{
	class SystemManager
	{
	public:
		AquaAPI static void Initialize();
		AquaAPI static void Destroy();

		template<typename T>
		static T* Add()
		{
			if (!std::is_base_of<Systems::System, T>())
				return nullptr;

			T* system = new T();
			m_Systems.emplace(typeid(T), system);
			return system;
		}

		template<typename T>
		static void Remove()
		{
			if (!Has<T>())
				return;
			std::type_index type = typeid(T);
			delete m_Systems[type];
			m_Systems.erase(type);
		}

		template<typename T>
		static bool Has() { return m_Systems.find(typeid(T)) != m_Systems.end(); }

		template<typename T>
		/// <returns>Pointer to system, or nullptr if not available</returns>
		static T* Get()
		{
			if (!Has<T>())
				return nullptr;
			return (T*)m_Systems[typeid(T)];
		}

		AquaAPI static void Draw();
		AquaAPI static void Update();

		AquaAPI static std::vector<Systems::System*> All();

	private:
		AquaAPI static std::unordered_map<std::type_index, Systems::System*> m_Systems;
	};
}
