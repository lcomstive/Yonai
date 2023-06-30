#pragma once
#include <memory>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>

namespace AquaEngine
{
	class Resource
	{
		struct ResourceInstance
		{
			void* Data;
			std::type_index Type;

			ResourceInstance() : Data(nullptr), Type(typeid(void)) { }
			ResourceInstance(ResourceInstance& other) : Data(other.Data), Type(other.Type) { }
			ResourceInstance(const ResourceInstance& other) : Data(other.Data), Type(other.Type) { }
			ResourceInstance(void* data, std::type_index& type) : Data(data), Type(type) { }

			ResourceInstance& operator =(ResourceInstance& other)
			{
				Data = other.Data;
				Type = other.Type;
				return *this;
			}
		};

		AquaAPI static std::vector<ResourceInstance> s_Instances;

		/// <summary>
		/// Links a string (often filepath) to a valid ResourceID
		/// </summary>
		AquaAPI static std::unordered_map<std::string, ResourceID> s_InstancePaths;

		/// <summary>
		/// Links a ResourceID to an index in s_Instances
		/// </summary>
		AquaAPI static std::unordered_map<ResourceID, size_t> s_ResourceIDs;

		AquaAPI static ResourceInstance* GetInstance(ResourceID id);

	public:
		template<typename T, class... ArgTypes>
		static ResourceID Load(std::string path, ArgTypes... constructorArgs)
		{
			return Load<T, ArgTypes...>(ResourceID(), path, constructorArgs...);
		}
		
		template<typename T, class... ArgTypes>
		static ResourceID Load(ResourceID id, std::string path, ArgTypes... constructorArgs)
		{
			replace(path.begin(), path.end(), '\\', '/');

			std::type_index loadType = typeid(T);

			// Check if instance already exists
			auto cacheIt = s_InstancePaths.find(path);
			if (cacheIt != s_InstancePaths.end())
			{
				ResourceInstance* loadedInstance = GetInstance(cacheIt->second);
				if (loadedInstance->Type != loadType)
				{
					spdlog::error("Tried loading cached resource '{}' of type '{}' but expected '{}'", path, loadedInstance->Type.name(), loadType.name());
					return InvalidResourceID;
				}
				return cacheIt->second;
			}

			// Create new resource
			s_Instances.emplace_back(ResourceInstance(new T(constructorArgs...), loadType));
			s_InstancePaths.emplace(path, id);
			s_ResourceIDs.emplace(id, s_Instances.size() - 1);

#if 1
			spdlog::trace("Created new resource '{}' [{}][{}]", path.c_str(), loadType.name(), id);
#endif

			return id;
		}

		template<typename T, class... ArgTypes>
		static T* LoadPtr(std::string path, ArgTypes... constructorArgs)
		{
			return Get<T>(Load<T>(path, constructorArgs...));
		}

		/// <summary>
		/// Duplicates a resource at the new given path
		/// </summary>
		AquaAPI static ResourceID Duplicate(ResourceID original, std::string newPath);

		/// <summary>
		/// Gets the C++ type related to a resource, or type of void if not found
		/// </summary>
		AquaAPI static std::type_index GetType(ResourceID id);

		/// <summary>
		/// Generic getter for resources whose types have a parameterless constructor
		/// </summary>
		template<typename T>
		static T* Get(std::string path)
		{
			if (!Exists(path))
				return (T*)nullptr;
			ResourceInstance* instance = GetInstance(s_InstancePaths[path]);
			return (T*)(instance && instance->Type == typeid(T) ? instance->Data : nullptr);
		}

		template<typename T>
		static T* Get(ResourceID id)
		{
			ResourceInstance* instance = GetInstance(id);
			return (T*)(instance && instance->Type == typeid(T) ? instance->Data : nullptr);
		}

		AquaAPI static void* Get(ResourceID id);
		AquaAPI static void* Get(std::string path);

		/// <summary>
		/// Gets the ID of resource at path, or InvalidResourceID if not found
		/// </summary>
		AquaAPI static ResourceID GetID(const std::string& path);

		/// <summary>
		/// Gets the path of a resource, or empty if not valid
		/// </summary>
		AquaAPI static std::string GetPath(ResourceID id);

		/// <summary>
		/// Releases resources associated with ResourceID
		/// </summary>
		AquaAPI static void Unload(ResourceID resource);

		/// <summary>
		/// Releases all associated resources
		/// </summary>
		AquaAPI static void UnloadAll();

		AquaAPI static bool Exists(std::string path);
		AquaAPI static bool IsValidResourceID(ResourceID id);

		template<typename T>
		static bool IsValidType(ResourceID id)
		{
			if (!IsValidResourceID(id))
				return false;
			ResourceInstance* instance = GetInstance(id);
			return instance && instance->Type == typeid(T);
		}

		AquaAPI static void PrintResourceTypes();

		template<typename T>
		static void PrintResourceTypes()
		{
			const std::type_info& typeInfo = typeid(T);
			spdlog::trace("Printing resources [{}]", typeInfo.name());
			for (auto& pair : s_InstancePaths)
			{
				ResourceInstance* instance = GetInstance(pair.second);
				if (instance->Type == typeInfo)
					spdlog::trace(" [{}->{}] {}", pair.second, s_ResourceIDs[pair.second], pair.first);
			}
		}
	};
}
