#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <Yonai/World.hpp>
#include <mono/metadata/assembly.h>

namespace Yonai
{
	namespace Components { struct Component; }
}

namespace Yonai::Scripting
{
	// Forward declaration for 'friend' below
	class ScriptEngine;
	struct Class;

	struct Assembly
	{
		struct ManagedComponentData
		{
			size_t Type = 0;

			/// <summary>
			/// Component* (World* world, unsigned int entityID).
			/// Returns created instance of component matching type <see cref="Type" />
			/// </summary>
			std::function<Components::Component* (World*, UUID)> AddFn = nullptr;
		};

		struct ManagedSystemData
		{
			size_t Type = 0;

			/// <summary>
			/// System* (World* world).
			/// Returns created instance of system matching type <see cref="Type" />
			/// </summary>
			std::function<Systems::System* (SystemManager*)> AddFn = nullptr;
		};

	private:
		/// <summary>
		/// Key:	Managed type index,					generated by Mono
		/// Value:  Struct containing hash of
		///				unmanaged (C++) component (hash code of type_info),
		///				and function for when component is added to an entity
		/// </summary>
		YonaiAPI static std::unordered_map<size_t, ManagedSystemData> s_InternalManagedSystemTypes;
		YonaiAPI static std::unordered_map<size_t, ManagedComponentData> s_InternalManagedComponentTypes;

		std::vector<MonoClass*> m_ManagedSystemTypes = {};
		std::vector<MonoClass*> m_ManagedComponentTypes = {};

		/// Maps YonaiScriptCore managed to unmanaged components (e.g. Transform)
		void LoadScriptCoreTypes();
		
		Assembly(MonoAssembly* handle, bool isCoreAssembly);
		
		friend class Yonai::Scripting::ScriptEngine;

	public:
		MonoImage* Image;
		MonoAssembly* Handle;

		YonaiAPI MonoClass* GetClassFromName(const char* namespaceName, const char* className, bool useClassCache = true);
		YonaiAPI MonoType* GetTypeFromClassName(const char* namespaceName, const char* className);

		YonaiAPI std::unique_ptr<Class> InstantiateClass(const char* namespaceName, const char* className);

		YonaiAPI static size_t GetTypeHash(MonoType* type);
		YonaiAPI static MonoType* GetTypeFromHash(size_t hash);
		YonaiAPI static size_t GetTypeHash(MonoClass* monoClass);
		YonaiAPI static size_t GetTypeHash(std::type_index& type);
		YonaiAPI static size_t GetUnmanagedHash(size_t managedHash);
		YonaiAPI static ManagedSystemData GetManagedSystemData(size_t unmanagedType);
		YonaiAPI static ManagedComponentData GetManagedComponentData(size_t unmanagedType);

		template<typename T>
		/// <summary>
		/// Binds a managed system to an unmanaged system.
		/// Once binding is complete, any operations performed on the managed system (e.g. added, removed) is also done to the unmanaged system in the same world.
		/// </summary>
		void BindManagedSystem(const char* managedNamespace, const char* managedName)
		{
			size_t managedHash = 0;
			size_t hash = typeid(T).hash_code();
			if (!AddInternalManagedType(hash, managedNamespace, managedName, &managedHash))
				return;

			s_InternalManagedSystemTypes.emplace(
				managedHash,
				ManagedSystemData
				{
					hash,
					[](SystemManager* systemManager) -> Systems::System* { return systemManager->Add<T>(); }
				}
			);
		}

		template<typename T>
		/// <summary>
		/// Binds a managed component to an unmanaged component.
		/// Once binding is complete, any operations performed on the component (e.g. added, removed) is also done to the unmanaged component on the same entity.
		/// </summary>
		void BindManagedComponent(const char* managedNamespace, const char* managedName)
		{
			size_t managedHash = 0;
			size_t hash = typeid(T).hash_code();
			if (!AddInternalManagedType(hash, managedNamespace, managedName, &managedHash))
				return;

			s_InternalManagedComponentTypes.emplace(
				managedHash,
				ManagedComponentData
				{
					hash,
					[](World* world, EntityID entityID) -> Components::Component* { return world->AddComponent<T>(entityID); }
				}
			);
		}

	private:
		static std::unordered_map<MonoType*, size_t> s_TypeHashes;
		static std::unordered_map<size_t, MonoType*> s_ReverseTypeHashes;
		
		/// <summary>
		/// Hashes all types.
		/// Stores classes deriving Yonai.YonaiSystem in <see cref="m_ManagedSystemTypes" />.
		/// Stores classes deriving Yonai.Component in <see cref="m_ManagedComponentTypes" />
		/// </summary>
		void CacheTypes(bool isCore);

		// Clears s_TypeHashes & s_ReverseTypeHashes.
		// Intended to be used when reloading assemblies
		static void ClearCachedTypes();

		YonaiAPI bool AddInternalManagedType(size_t unmanagedTypeHash, const char* managedNamespace, const char* managedName, size_t* managedHash)
		{
			MonoClass* klass = mono_class_from_name(Image, managedNamespace, managedName);
			if (!klass)
			{
				spdlog::warn("Failed to add internal managed type definition for '{}' - not found in assembly '{}'", managedName, mono_image_get_name(Image));
				return false;
			}

			MonoType* managedType = mono_class_get_type(klass);
			*managedHash = GetTypeHash(managedType);

			// Store both native and non-native hashes of this type to the managed (C#) MonoType*
			s_TypeHashes.emplace(managedType, *managedHash);
			s_ReverseTypeHashes.emplace(unmanagedTypeHash, managedType);
			s_ReverseTypeHashes.emplace(*managedHash, managedType);

			return true;
		}
	};
}