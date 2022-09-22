#pragma once
#include <string>
#include <vector>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <AquaEngine/API.hpp>
#include <AquaEngine/IO/FileWatcher.hpp>

#ifdef AQUA_PLATFORM_WINDOWS
#undef CreateDirectory
#endif

namespace AquaEngine::IO
{
	typedef std::function<void(std::string, FileWatchStatus)> VFSMappingCallback;

	enum class FilePermissions : unsigned int
	{
		None = 0x00,
		Read = 0x01,
		Write = 0x02,

		ReadWrite = Read | Write
	};

	struct VFSFile
	{
		/// <summary>
		/// Name of file, including extension
		/// </summary>
		std::string Filename = "";

		/// <summary>
		/// Is this file a directory?
		/// </summary>
		bool Directory = false;

		/// <summary>
		/// Path to directory containing file in VFS
		/// </summary>
		std::string ParentDirectory = "/";

		/// <summary>
		/// Access level to file
		/// </summary>
		FilePermissions Permissions = FilePermissions::None;

		std::string FullPath() { return ParentDirectory + Filename; }
	};

	class VFSMapping
	{
	private:
		std::string m_MountPoint, m_MountPath;

	protected:
		std::unordered_map<std::string, VFSMappingCallback> m_Callbacks;

	public:
		AquaAPI VFSMapping(std::string mountPoint, std::string mountPath);
		AquaAPI virtual ~VFSMapping() { }

		/// <summary>
		/// Path that mapping was mounted to
		/// </summary>
		AquaAPI std::string GetMountPath();

		/// <summary>
		/// Mounting point
		/// </summary>
		AquaAPI std::string GetMountPoint();

		/// <returns>Mounted path, or the same path if different mount prefix was found</returns>
		AquaAPI std::string GetMountedPath(std::string path);

		/// <returns>True if file exists and can be retrieved</returns>
		AquaAPI virtual bool Exists(std::string path) = 0;

		/// <returns>True if directory was created successfully<returns>
		AquaAPI virtual bool CreateDirectory(std::string path);

		/// <returns>True if directory was deleted successfully<returns>
		AquaAPI virtual bool DeleteDirectory(std::string path);

		/// <summary>
		/// Reads text from a file
		/// </summary>
		AquaAPI virtual std::string ReadText(std::string path) = 0;

		/// <summary>
		/// Reads binary from a file
		/// </summary>
		AquaAPI virtual std::vector<unsigned char> Read(std::string path) = 0;

		/// <summary>
		/// Returns names of all files and directories in directory
		/// </summary>
		AquaAPI std::vector<VFSFile> GetFiles(std::string directory, bool recursive = false);

		/// <summary>
		/// Returns names of all files and directories in directory
		/// </summary>
		AquaAPI virtual void GetFiles(std::string directory, std::vector<VFSFile>& files, bool recursive = false) { }

		/// <summary>
		/// Writes text to file
		/// </summary>
		AquaAPI virtual void WriteText(std::string path, std::string text, bool append = false) { }

		/// <summary>
		/// Writes binary to file
		/// </summary>
		AquaAPI virtual void Write(std::string path, std::vector<unsigned char> data, bool append = false) { }

		/// <summary>
		/// Copies a file
		/// </summary>
		AquaAPI virtual void Copy(std::string originalPath, std::string newPath) { }

		AquaAPI virtual bool CanWrite();
		AquaAPI virtual bool CanWatch();
		AquaAPI virtual FilePermissions GetPermissions();

		/// <summary>
		/// Begins monitoring a file for changes. Only one callback can be added per path
		/// </summary>
		AquaAPI virtual void Watch(std::string path, VFSMappingCallback callback) { }

		/// <summary>
		/// Stops monitoring a file for changes
		/// </summary>
		AquaAPI virtual void Unwatch(std::string path) { }
	};
}
