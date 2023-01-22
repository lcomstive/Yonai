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
		AquaAPI static std::unordered_map<std::string, std::list<AquaEngine::IO::VFSMapping*>> s_Mappings;

	public:
		AquaAPI static std::filesystem::path GetCurrentDirectory();

		/// <returns>True if mountPoint exists</returns>
		AquaAPI static bool HasMount(std::string mountPoint);

		/// <summary>
		/// Mounts mountPoint to a local directory of the same name
		/// </summary>
		/// <returns>Created mapping, or nullptr if failed</returns>
		AquaAPI static VFSMapping* Mount(std::string mountPoint);

		/// <summary>
		/// Mounts a path with alias mountPoint
		/// </summary>
		/// <returns>Created mapping, or nullptr if failed</returns>
		AquaAPI static VFSMapping* Mount(std::string mountPoint, std::string mountPath);

		/// <summary>
		/// Removes all aliases for point
		/// </summary>
		AquaAPI static void Unmount(std::string mountPoint);

		/// <summary>
		/// Removes first alias that matches path
		/// </summary>
		AquaAPI static void Unmount(std::string mountPoint, std::string mountPath);

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
				return Mount<T>(mountPoint, mountPoint);

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
			if (!mountPoint.empty())
			{
				// Replace all backslashes (Windows style) to forward slashes
				replace(mountPoint.begin(), mountPoint.end(), '\\', '/');

				// Remove end slashes
				if (mountPoint[mountPoint.size() - 1] == '/' && mountPoint.size() > 1)
					mountPoint.erase(mountPoint.size() - 1);
			}

			// Configure mountPath
			mountPath = VFS::GetAbsolutePath(mountPath);
			if (!mountPath.empty() && mountPath[mountPath.size() - 1] == '/')
				mountPath.erase(mountPath.size() - 1);

			// Check that template type derives from the VFSMapping class
			if (!std::is_base_of<VFSMapping, T>())
			{
				spdlog::error("Cannot mount '{}' to '{}' - invalid mount type '{}'", mountPoint, mountPath, typeid(T).name());
				return nullptr;
			}

			// Add list if the mount point hasn't been used before
			if (s_Mappings.find(mountPoint) == s_Mappings.end())
				s_Mappings.emplace(mountPoint, std::list<VFSMapping*>());

			// Add the mounting
			T* instance = new T(mountPoint, mountPath);
			s_Mappings[mountPoint].emplace_back(instance);

			spdlog::debug("Mounted '{}' to '{}'", mountPoint, mountPath);
			return instance;
		}

		AquaAPI static std::string ReadText(const std::string& path);
		AquaAPI static std::vector<unsigned char> Read(const std::string& path);

		AquaAPI static void WriteText(const std::string& path, std::string contents);
		AquaAPI static void Write(const std::string& path, std::vector<unsigned char> contents);

		AquaAPI static void Remove(const std::string& path);
		AquaAPI static void Move(const std::string& originalPath, const std::string& newPath);
		AquaAPI static void Copy(const std::string& originalPath, const std::string& copyPath);

		/// <summary>
		/// Gets an absolute path from the first appropriate mount
		/// </summary>
		AquaAPI static std::string GetAbsolutePath(std::string path);

		/// <param name="needExistingFile">Does the path need to exist inside the mapping? When false, only returns writeable mappings</param>
		AquaAPI static VFSMapping* GetMapping(std::string path, bool needExistingFile = true, FilePermissions requiredPerms = FilePermissions::ReadWrite);

		/// <param name="needExistingFile">Does the path need to exist inside the mapping? When false, only returns writeable mappings</param>
		AquaAPI static std::vector<VFSMapping*> GetMappings(std::string path, bool needExistingFile = true, FilePermissions requiredPerms = FilePermissions::ReadWrite);

		AquaAPI static bool Exists(std::string path);

		AquaAPI static std::vector<VFSFile> GetFiles(std::string directory, bool recursive = false);
	};
}
