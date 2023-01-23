#include <filesystem>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/IO/VFS/VFSPhysicalFile.hpp>

using namespace std;
using namespace AquaEngine::IO;

namespace fs = std::filesystem;

unordered_map<string, list<VFSMapping*>> VFS::s_Mappings;

VFSMapping* VFS::Mount(string mountPoint) { return Mount<VFSPhysicalFileMapping>(mountPoint); }
VFSMapping* VFS::Mount(string mountPoint, string mountPath) { return Mount<VFSPhysicalFileMapping>(mountPoint, mountPath); }

void VFS::Unmount(string mountPoint, string mountPath)
{
	if (s_Mappings.find(mountPoint) == s_Mappings.end())
		return; // Mount point isn't mapped

	for (auto it : s_Mappings[mountPoint])
	{
		if (it->GetMountPath().compare(mountPath) == 0)
		{
			s_Mappings[mountPoint].remove(it);
			delete it;
			break;
		}
	}
}

filesystem::path VFS::GetCurrentDirectory() { return filesystem::current_path(); }
bool VFS::HasMount(string mountPath) { return s_Mappings.find(mountPath) != s_Mappings.end(); }

bool VFS::Exists(string path)
{
	for (auto& mounts : s_Mappings)
	{
		// Check if path starts with mount
		if (path.rfind(mounts.first, 0) != 0)
			continue;
		for (auto& mapping : mounts.second)
			if (mapping->Exists(mapping->GetMountedPath(path)))
				return true;
	}
	return fs::exists(path);
}

VFSMapping* VFS::GetMapping(string path, bool needExistingFile, FilePermissions requiredPerms)
{
	for (auto& mounts : s_Mappings)
	{
		// Check if path starts with mount
		if (path.rfind(mounts.first, 0) != 0)
			continue;
		for (auto& mapping : mounts.second)
		{
			if (!needExistingFile && ((unsigned int)mapping->GetPermissions() & (unsigned int)requiredPerms) != 0) // Valid mapping found
				return mapping;
			if (needExistingFile && mapping->Exists(mapping->GetMountedPath(path))) // Valid file found inside mapping
				return mapping;
		}
	}
	return nullptr;
}

vector<VFSMapping*> VFS::GetMappings(string path, bool needExistingFile, FilePermissions requiredPerms)
{	
	vector<VFSMapping*> mappings;

	for (auto& mounts : s_Mappings)
	{
		// Check if path starts with mount
		if (path.rfind(mounts.first, 0) != 0)
			continue;
		for (auto& mapping : mounts.second)
		{
			if (!needExistingFile && ((unsigned char)mapping->GetPermissions() & (unsigned char)requiredPerms) != 0) // Valid writeable mapping found
				mappings.emplace_back(mapping);
			if (needExistingFile && mapping->Exists(mapping->GetMountedPath(path))) // Valid file found inside mapping
				mappings.emplace_back(mapping);
		}
	}
	return mappings;
}

void VFS::Unmount(string mountPoint)
{
	if (s_Mappings.find(mountPoint) == s_Mappings.end())
		return;

	for (auto it : s_Mappings[mountPoint])
		delete it;
	s_Mappings[mountPoint].clear();
}

string VFS::ReadText(const string& path)
{
	VFSMapping* mapping = GetMapping(path);
	if (!mapping)
		spdlog::warn("Could not find virtual mapping to suit '{}'", path);
	return mapping ? mapping->ReadText(mapping->GetMountedPath(path)) : "";
}

vector<unsigned char> VFS::Read(const string& path)
{
	VFSMapping* mapping = GetMapping(path);
	if (!mapping)
		spdlog::warn("Could not find virtual mapping to suit '{}'", path);
	return mapping ? mapping->Read(mapping->GetMountedPath(path)) : vector<unsigned char>();
}

void VFS::WriteText(const string& path, string contents)
{
	VFSMapping* mapping = GetMapping(path, false, FilePermissions::Write);

	if (!mapping)
	{
		spdlog::warn("Could not find virtual mapping to suit '{}'", path);
		return;
	}

	mapping->WriteText(mapping->GetMountedPath(path), contents);
}

void VFS::Write(const string& path, vector<unsigned char> contents)
{
	VFSMapping* mapping = GetMapping(path, false, FilePermissions::Write);
	if (!mapping)
	{
		spdlog::warn("Could not find virtual mapping to suit '{}'", path);
		return;
	}

	mapping->Write(mapping->GetMountedPath(path), contents);
}

void VFS::Remove(const string& path)
{
	VFSMapping* mapping = GetMapping(path, false, FilePermissions::Write);
	if (!mapping)
	{
		spdlog::warn("Could not find virtual mapping to suit '{}'", path);
		return;
	}

	mapping->Remove(mapping->GetMountedPath(path));
}

void VFS::Move(const string& originalPath, const string& newPath)
{
	VFSMapping* originalMapping = GetMapping(originalPath, false, FilePermissions::Read);
	VFSMapping* newMapping = GetMapping(newPath, false, FilePermissions::Write);
	if (!originalMapping)
	{
		spdlog::warn("Could not find virtual mapping to suit '{}'", originalPath);
		return;
	}
	if (!newMapping)
	{
		spdlog::warn("Could not find virtual mapping to suit '{}'", newPath);
		return;
	}

	originalMapping->Move(originalMapping->GetMountedPath(originalPath), newMapping->GetMountedPath(newPath));
}

void VFS::Copy(const string& originalPath, const string& copyPath)
{
	VFSMapping* originalMapping = GetMapping(originalPath, false, FilePermissions::Read);
	VFSMapping* newMapping = GetMapping(copyPath, false, FilePermissions::Write);
	if (!originalMapping)
	{
		spdlog::warn("Could not find virtual mapping to suit '{}'", originalPath);
		return;
	}
	if (!newMapping)
	{
		spdlog::warn("Could not find virtual mapping to suit '{}'", copyPath);
		return;
	}

	originalMapping->Copy(originalMapping->GetMountedPath(originalPath), newMapping->GetMountedPath(copyPath));
}

vector<VFSFile> VFS::GetFiles(string directory, bool recursive)
{
	vector<VFSFile> files;
	for (auto& mapping : GetMappings(directory))
		mapping->GetFiles(directory, files, recursive);
	return files;
}

string VFS::GetAbsolutePath(string path, bool suppressWarning)
{
	VFSMapping* mapping = GetMapping(path, false);
	if (mapping)
		return mapping->GetMountedPath(path);

	if(!suppressWarning)
		spdlog::warn("Could not find virtual mapping to suit '{}'", path);

	return path;
}