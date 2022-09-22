#pragma once
#include <unordered_map>
#include <AquaEngine/IO/FileWatcher.hpp>
#include <AquaEngine/IO/VFS/VFSMapping.hpp>

namespace AquaEngine::IO
{
	class VFSPhysicalFileMapping : public VFSMapping
	{
		std::unordered_map<std::string, FileWatcher*> m_Watchers;

		void AddDirectoryFiles(std::string directory, bool recursive, std::vector<VFSFile>& files);

	public:
		AquaAPI VFSPhysicalFileMapping(std::string mountPoint, std::string path);
		AquaAPI ~VFSPhysicalFileMapping();

		AquaAPI virtual std::string ReadText(std::string path) override;
		AquaAPI virtual std::vector<unsigned char> Read(std::string path) override;
		AquaAPI virtual void GetFiles(std::string directory, std::vector<VFSFile>& files, bool recursive = false) override;

		AquaAPI virtual void WriteText(std::string path, std::string text, bool append = false) override;
		AquaAPI virtual void Write(std::string path, std::vector<unsigned char> data, bool append = false) override;

		AquaAPI virtual bool CreateDirectory(std::string path) override;
		AquaAPI virtual bool DeleteDirectory(std::string path) override;

		AquaAPI virtual void Copy(std::string originalPath, std::string newPath) override;

		AquaAPI virtual void Watch(std::string path, VFSMappingCallback callback) override;
		AquaAPI virtual void Unwatch(std::string path) override;

		AquaAPI virtual bool CanWrite() override;
		AquaAPI virtual bool CanWatch() override;
		AquaAPI virtual bool Exists(std::string path) override;
		AquaAPI virtual FilePermissions GetPermissions() override;
	};
}
