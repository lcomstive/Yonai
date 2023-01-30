#include <regex>
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS/VFSPhysicalFile.hpp>

using namespace std;
using namespace AquaEngine::IO;

namespace fs = std::filesystem;

VFSPhysicalFileMapping::VFSPhysicalFileMapping(string mountPoint, string path) : VFSMapping::VFSMapping(mountPoint, path)
{
	if(!path.empty() && !fs::exists(path))
		fs::create_directories(path);
}

VFSPhysicalFileMapping::~VFSPhysicalFileMapping()
{
	for (auto& it : m_Watchers)
	{
		it.second->Stop();
		delete it.second;
	}
	m_Watchers.clear();
}

string VFSPhysicalFileMapping::ReadText(string path)
{
	if(!fs::exists(path) || fs::is_directory(path))
		return "";

	// Open file for read and get size
	ifstream filestream(path, ios::in | ios::binary);

	// Get file length
	streampos size = fs::file_size(path);

	// Read in contents of file
	string contents(size, '\0');
	filestream.read(contents.data(), size);

	// Convert CRLF -> LF
	contents = regex_replace(contents, regex("\r\n"), "\n");

	return contents;
}

vector<unsigned char> VFSPhysicalFileMapping::Read(string path)
{
	ifstream filestream(path, ios::in | ios::binary);

	// Get file length
	filestream.seekg(0, ios::end);
	size_t filesize = filestream.tellg();

	filestream.seekg(0, ios::beg); // Go back to start

	// Prepare vector
	vector<unsigned char> contents(filesize);

	filestream.read((char*)&contents[0], filesize);
	filestream.close();

	return contents;
}

void VFSPhysicalFileMapping::AddDirectoryFiles(string directory, bool recursive, vector<VFSFile>& files)
{
	replace(directory.begin(), directory.end(), '\\', '/');
	string mountedPath = GetMountedPath(directory);
	if (!fs::exists(mountedPath) || !fs::is_directory(mountedPath))
		return;
	if (directory[directory.size() - 1] != '/')
		directory += "/";

	for (const auto& entry : fs::directory_iterator(mountedPath))
	{
		fs::path path = entry.path();
		VFSFile file =
		{
			path.filename().u8string(),
			fs::is_directory(path),
			directory
		};

		fs::perms permissions = entry.status().permissions();
		bool canRead  = (permissions & fs::perms::owner_read)  != fs::perms::none;
		bool canWrite = (permissions & fs::perms::owner_write) != fs::perms::none;
		
		if (canRead && canWrite)
			file.Permissions = FilePermissions::ReadWrite;
		else if (canRead)
			file.Permissions = FilePermissions::Read;

		files.emplace_back(file);

		if (recursive && file.Directory)
			AddDirectoryFiles(file.FullPath(), recursive, files);
	}
}

void VFSPhysicalFileMapping::GetFiles(std::string directory, vector<VFSFile>& files, bool recursive)
{
	AddDirectoryFiles(directory, recursive, files);
}

void VFSPhysicalFileMapping::WriteText(string path, string text, bool append)
{	
	// Check if parent directory already exists
	fs::path parentDir = fs::path(path).parent_path();
	if (!fs::exists(parentDir) && !fs::create_directories(parentDir))
	{
		spdlog::error("Failed to write to '{}' - could not create parent directory", path);
		return;
	}

	ofstream filestream(path, ios::out | (append ? ios::app : ios::trunc));
	filestream.write(text.c_str(), text.length());
	filestream.flush();
	filestream.close();
}

void VFSPhysicalFileMapping::ReplaceText(string path, const string& from, const string& to)
{
	if (from.empty() || !Exists(path))
		return;

	string content = ReadText(path);

	size_t startPos = 0;
	while ((startPos = content.find(from, startPos)) != string::npos)
	{
		content.replace(startPos, from.length(), to);
		startPos += to.length();
	}

	WriteText(path, content);
}

void VFSPhysicalFileMapping::ReplaceText(string path, vector<pair<string, string>> pairs)
{
	string content = ReadText(path);
	for(auto pair : pairs)
		ReplaceText(content, pair.first, pair.second);
	WriteText(path, content);
}

void VFSPhysicalFileMapping::Write(string path, vector<unsigned char> data, bool append)
{
	fs::path parentDir = fs::path(path).parent_path();
	if (!fs::is_directory(parentDir) && !fs::create_directories(parentDir))
	{
		spdlog::error("Failed to write to '{}' - could not create parent directory", path);
		return;
	}

	ofstream filestream(path, ios::out | ios::binary | (append ? ios::app : ios::trunc));
	filestream.write((char*)data.data(), data.size());
	filestream.flush();
	filestream.close();
}

void VFSPhysicalFileMapping::Copy(string originalPath, string newPath)
{
	// Check new path parent directory
	fs::path parentDir = fs::path(newPath).parent_path();
	if (!fs::is_directory(parentDir) && !fs::create_directories(parentDir))
	{
		spdlog::error("Failed to write to '{}' - could not create parent directory", newPath);
		return;
	}

	fs::copy(originalPath, newPath, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
}

void VFSPhysicalFileMapping::Remove(string path)
{
	if(fs::exists(path))
		fs::remove_all(path);
}
	
void VFSPhysicalFileMapping::Move(string originalPath, string newPath)
{
	if(fs::exists(originalPath))
		fs::rename(originalPath, newPath);
}

void VFSPhysicalFileMapping::Watch(string path, VFSMappingCallback callback)
{
	if (m_Watchers.find(path) != m_Watchers.end())
		return; // Already being watched

	// Physical file watcher
	m_Watchers.emplace(path, new FileWatcher(GetMountedPath(path), true));
	m_Watchers[path]->Start(callback);
}

void VFSPhysicalFileMapping::Unwatch(string path)
{
	if (m_Watchers.find(path) == m_Watchers.end())
		return; // Not being watched

	m_Watchers[path]->Stop();
	delete m_Watchers[path];
	m_Watchers.erase(path);
}

bool VFSPhysicalFileMapping::CreateDirectory(std::string path) { return fs::create_directories(path); }
bool VFSPhysicalFileMapping::DeleteDirectory(std::string path) { return fs::remove_all(path); }

bool VFSPhysicalFileMapping::CanWrite() { return true; }
bool VFSPhysicalFileMapping::CanWatch() { return true; }
bool VFSPhysicalFileMapping::Exists(string path) { return fs::exists(path); }
FilePermissions VFSPhysicalFileMapping::GetPermissions() { return FilePermissions::ReadWrite; }
