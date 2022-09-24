#include <regex>
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/Files.hpp>

using namespace std;
using namespace AquaEngine::IO;

namespace fs = std::filesystem;

string AquaEngine::IO::ReadText(string path)
{
	if (!fs::exists(path))
	{
		spdlog::warn("Cannot read '{}' - file does not exist", path);
		return "";
	}

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

vector<unsigned char> AquaEngine::IO::Read(string path)
{
	if (!fs::exists(path))
	{
		spdlog::warn("Cannot read '{}' - file does not exist", path);
		return vector<unsigned char>();
	}

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

void AquaEngine::IO::Write(string path, string& contents)
{
	ofstream filestream(path, ios::out | ios::binary);
	filestream.write(contents.c_str(), contents.size());
	filestream.flush();
	filestream.close();
}

void AquaEngine::IO::Write(string path, vector<unsigned char>& data)
{
	ofstream filestream(path, ios::out | ios::binary);
	filestream.write((const char*)data.data(), data.size());
	filestream.flush();
	filestream.close();
}

bool AquaEngine::IO::Exists(std::string path) { return fs::exists(path); }
