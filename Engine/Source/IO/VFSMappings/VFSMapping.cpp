#include <AquaEngine/IO/VFS/VFSMapping.hpp>

using namespace std;
using namespace AquaEngine::IO;

VFSMapping::VFSMapping(string mountPoint, string mountPath) : m_MountPoint(mountPoint), m_MountPath(mountPath) { }

string VFSMapping::GetMountPath() { return m_MountPath; }
string VFSMapping::GetMountPoint() { return m_MountPoint; }

bool VFSMapping::CanWrite() { return false; }
bool VFSMapping::CanWatch() { return false; }
FilePermissions VFSMapping::GetPermissions() { return FilePermissions::None; }

bool VFSMapping::CreateDirectory(string path) { return false; }
bool VFSMapping::DeleteDirectory(string path) { return false; }

string VFSMapping::GetMountedPath(string path)
{
	if (path.rfind(GetMountPoint()) == 0)
		return GetMountPath() + path.erase(0, GetMountPoint().size());
	else
		return path;
}

vector<VFSFile> VFSMapping::GetFiles(string directory, bool recursive)
{
	vector<VFSFile> files;
	GetFiles(directory, files, recursive);
	return files;
}