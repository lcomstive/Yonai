#pragma once
#include <string>
#include <vector>

namespace AquaEditor
{
	struct ProjectInfo
	{
		std::string Name;
		std::string AssetDir;
		std::vector<std::string> Assemblies;
	};

	ProjectInfo ReadProject(const std::string& path);
}