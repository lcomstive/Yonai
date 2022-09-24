#pragma once
#include <list>
#include <string>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS/VFSMapping.hpp>

namespace AquaEngine::IO
{
	class VFS
	{
		// MountPoint, list of mappings in order of priority
		static std::unordered_map<std::string, std::list<AquaEngine::IO::VFSMapping*>> m_Mappings;

	public:
		static std::filesystem::path GetCurrentDirectory();

		/// <returns>True if mountPoint exists</returns>
		static bool HasMount(std::string mountPoint);

		/// <summary>
		/// Mounts mountPoint to a local directory of the same name
		/// </summary>
		/// <returns>Created mapping, or nullptr if failed</returns>
		static VFSMapping* Mount(std::string mountPoint);

		/// <summary>
		/// Mounts a path with alias mountPoint
		/// </summary>
		/// <returns>Created mapping, or nullptr if failed</returns>
		static VFSMapping* Mount(std::string mountPoint, std::string mountPath);

		/// <summary>
		/// Removes all aliases for point
		/// </summary>
		static void Unmount(std::string mountPoint);

		/// <summary>
		/// Removes first alias that matches path
		/// </summary>
		static void Unmount(std::string mountPoint, std::string mountPath);

		/// <summary>
		/// Creates an alias using local directory of same name
		/// </summary>
		/// <typeparam name="T">VFSMapping-derived class to use for file operations</typeparam>
		/// <param name="mountPoint">Alias name & local file or directory path</param>
		/// <returns>Created mapping, or nullptr if failed</returns>
		template<typename T>
		static T* Mount(std::string mountPoint)
		{
			if (mountPoint.empty())
				return nullptr;
			if (mountPoint[0] != '/' && mountPoint[0] != '\\')
				mountPoint = "/" + mountPoint;
			return Mount<T>(mountPoint, "." + mountPoint);
		}

		/// <summary>
		/// Creates an alias for a path
		/// </summary>
		/// <typeparam name="T">VFSMapping-derived class to use for file operations</typeparam>
		/// <param name="mountPoint">Alias name</param>
		/// <param name="mountPath">Local file or directory path</param>
		/// <returns>Created mapping, or nullptr if failed</returns>
		template<typename T>
		static T* Mount(std::string mountPoint, std::string mountPath)
		{
			// Check for valid parameters
			if (mountPoint.empty() || mountPath.empty())
				return nullptr;
			// Replace all backslashes (Windows style) to forward slashes
			replace(mountPoint.begin(), mountPoint.end(), '\\', '/');

			// Remove end slashes
			if (mountPoint[mountPoint.size() - 1] == '/' && mountPoint.size() > 1)
				mountPoint.erase(mountPoint.size() - 1);
			if (mountPath[mountPath.size() - 1] == '/' && mountPath.size() > 1)
				mountPath.erase(mountPath.size() - 1);

			// Check that template type derives from the VFSMapping class
			if (!std::is_base_of<VFSMapping, T>())
			{
				spdlog::error("Cannot mount '{}' to '{}' - invalid mount type '{}'", mountPoint, mountPath, typeid(T).name());
				return nullptr;
			}

			// Add list if the mount point hasn't been used before
			if (m_Mappings.find(mountPoint) == m_Mappings.end())
				m_Mappings.emplace(mountPoint, std::list<VFSMapping*>());

			// Add the mounting
			T* instance = new T(mountPoint, mountPath);
			m_Mappings[mountPoint].emplace_back(instance);

			spdlog::debug("Mounted '{}' to '{}'", mountPoint, mountPath);
			return instance;
		}

		static std::string ReadText(std::string path);
		static std::vector<unsigned char> Read(std::string path);

		static void WriteText(std::string path, std::string contents);
		static void Write(std::string path, std::vector<unsigned char> contents);

		static void Remove(std::string path);
		static void Move(std::string originalPath, std::string newPath);
		static void Copy(std::string originalPath, std::string copyPath);

		/// <summary>
		/// Gets an absolute path from the first appropriate mount
		/// </summary>
		static std::string GetAbsolutePath(std::string path);

		/// <param name="needExistingFile">Does the path need to exist inside the mapping? When false, only returns writeable mappings</param>
		static VFSMapping* GetMapping(std::string path, bool needExistingFile = true, FilePermissions requiredPerms = FilePermissions::ReadWrite);

		/// <param name="needExistingFile">Does the path need to exist inside the mapping? When false, only returns writeable mappings</param>
		static std::vector<VFSMapping*> GetMappings(std::string path, bool needExistingFile = true, FilePermissions requiredPerms = FilePermissions::ReadWrite);

		static bool Exists(std::string path);

		static std::vector<VFSFile> GetFiles(std::string directory, bool recursive = false);
	};
}
