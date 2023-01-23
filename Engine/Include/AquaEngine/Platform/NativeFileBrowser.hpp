#pragma once
#include <filesystem>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Platform
{
	AquaAPI std::filesystem::path OpenFolderDialog(std::string title);
	AquaAPI std::filesystem::path OpenFileDialog(std::string title, std::vector<std::string> extensionFilters = {});
	AquaAPI std::vector<std::filesystem::path> OpenFilesDialog(std::string title, std::vector<std::string> extensionFilters = {});
}