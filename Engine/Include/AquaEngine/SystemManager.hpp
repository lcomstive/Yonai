#pragma once
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Scripting/ManagedData.hpp>

namespace AquaEngine
{
	class World;
	namespace Systems { struct ScriptSystem; }
	namespace Scripting { class ScriptEngine; }

	class SystemManager
	{
		World* m_Owner;

		static SystemManager* s_Global;
		
		AquaAPI Scripting::ManagedData CreateManagedInstance(size_t typeHash);

	public:
		AquaAPI SystemManager(World* owner);

		AquaAPI void Enable(bool enable = true);
		AquaAPI void Destroy();

		AquaAPI void Draw();
		AquaAPI void Update();

		AquaAPI World* GetWorld();
		AquaAPI static SystemManager* Global();

		template<typename T>
		T* Add(size_t type)
		{
			if (!std::is_base_of<Systems::System, T>())
			{
				spdlog::warn("Cannot add system of type '{}' because it does not derive from AquaEngine::Systems::System",
					typeid(T).name());
				return nullptr;
			}

			T* instance = Get<T>();
			if (instance) // Check if system already added
				return instance;
			instance = new T();
			instance->m_Owner = this;

			Systems::System* system = (Systems::System*)instance;
			system->ManagedData = CreateManagedInstance(type);
			system->Init();

			m_Systems.emplace(type, instance);
			return instance;
		}

		template<typename T>
		T* Add() { return Add<T>(typeid(T).hash_code()); }

		AquaAPI Systems::ScriptSystem* Add(MonoType* managedType);

		template<typename T>
		bool Remove()
		{
			if (!Has<T>())
				return false;
			size_t type = typeid(T).hash_code();
			delete m_Systems[type];
			m_Systems.erase(type);
			return true;
		}

		template<typename T>
		bool Has() { return Has(typeid(T).hash_code()); }

		template<typename T>
		/// <returns>Pointer to system, or nullptr if not available</returns>
		T* Get() { return (T*)Get(typeid(T).hash_code()); }

		AquaAPI bool Has(size_t hash);
		AquaAPI bool Remove(size_t hash);
		AquaAPI Systems::System* Get(size_t hash);

		AquaAPI std::vector<Systems::System*> All();

	private:
		std::unordered_map<size_t, Systems::System*> m_Systems = {};

		void CreateAllManagedInstances();
		void InvalidateAllManagedInstances();

		friend class Scripting::ScriptEngine;
	};
}
