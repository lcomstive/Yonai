#pragma once
#include <string>
#include <vector>
#include <AquaEngine/API.hpp>

namespace AquaEngine::IO
{
	AquaAPI std::string ReadText(std::string path);
	AquaAPI std::vector<unsigned char> Read(std::string path);

	AquaAPI void Write(std::string path, std::string& contents);
	AquaAPI void Write(std::string path, std::vector<unsigned char>& contents);

	AquaAPI bool Exists(std::string path);
}