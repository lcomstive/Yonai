#pragma once
#include <string>
#include <vector>
#include <Yonai/API.hpp>

namespace Yonai::IO
{
	YonaiAPI std::string ReadText(std::string path);
	YonaiAPI std::vector<unsigned char> Read(std::string path);

	YonaiAPI void Write(std::string path, std::string& contents);
	YonaiAPI void Write(std::string path, std::vector<unsigned char>& contents);

	YonaiAPI bool Exists(std::string path);
}