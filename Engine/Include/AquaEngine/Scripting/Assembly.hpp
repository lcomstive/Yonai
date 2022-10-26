#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <mono/metadata/assembly.h>
#include <AquaEngine/Scripting/Class.hpp>
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine::Scripting
{
	class ScriptEngine;

	struct Assembly
	{
		struct ManagedComponentData
		{
			size_t Type;
			std::function<Components::Component* (World*, EntityID)> AddFn;
		};

	private:
		/// <summary>
		/// Key:	Managed type index,					generated by Mono
		/// Value:  Hash of unmanaged (C++) component,	hash code of type_info
		/// </summary>
		static std::unordered_map<size_t, ManagedComponentData> s_InternalManagedComponentTypes;

		friend class AquaEngine::Scripting::ScriptEngine;

		/// Maps AquaScriptCore managed to unmanaged components (e.g. Transform)
		void LoadScriptCoreTypes();

	public:
		MonoImage* Image;
		ScriptEngine* Owner;
		MonoAssembly* Handle;

		Assembly(ScriptEngine* owner, MonoAssembly* handle);

		std::unique_ptr<Class> InstantiateClass(const char* namespaceName, const char* className);

		static size_t GetTypeHash(MonoType* type);
		static ManagedComponentData GetManagedComponentData(size_t unmanagedType);

		template<typename T>
		static ManagedComponentData GetManagedComponentData()
		{ return GetManagedComponentData(typeid(T).hash_code()); }
	
	private:
		static std::unordered_map<MonoType*, size_t> s_TypeHashes;
		
		void HashTypes();
		void AddInternalCalls();

#pragma region Internal Calls
		void AddLogInternalCalls();
		void AddTimeInternalCalls();
		void AddVectorInternalCalls();
		void AddTransformInternalCalls();
		void AddWorldInternalCalls();
#pragma endregion

		template<typename T>
		void AddInternalManagedComponent(char* managedNamespace, char* managedName)
		{
			MonoClass* klass = mono_class_from_name(Image, managedNamespace, managedName);
			if (!klass)
			{
				spdlog::warn("Failed to add internal managed component definition for '{}' - not found in assembly '{}'", managedName, mono_image_get_name(Image));
				return;
			}

			s_InternalManagedComponentTypes.emplace(
				GetTypeHash(mono_class_get_type(klass)),
				ManagedComponentData
				{
					typeid(T).hash_code(),
					[](World* world, EntityID entityID) -> Components::Component* { return world->AddComponent<T>(entityID); }
				}
			);
		}
	};
}