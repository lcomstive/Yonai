#include <spdlog/spdlog.h>
#include <ProjectInfo.hpp>
#include <rapidjson/document.h>
#include <AquaEngine/IO/VFS.hpp>

using namespace std;
using namespace rapidjson;
using namespace AquaEditor;
using namespace AquaEngine::IO;

ProjectInfo AquaEditor::ReadProject(const std::string& path)
{
	spdlog::debug("Reading project file '{}'", VFS::GetAbsolutePath(path).c_str());

	ProjectInfo info = {};
	if(!VFS::Exists(path))
	{
		spdlog::warn("Cannot open project file - file could not be found");
		return info;
	}

	Document json;
	json.Parse(VFS::ReadText(path).c_str());

	const char* requiredFields[] = { "name", "assetsDir", "assemblies" };
	for(int i = 0; i < 3; i++)
	{
		if(!json.HasMember(requiredFields[i]))
		{
			spdlog::error("Cannot open project file - required field '{}' not present", requiredFields[i]);
			return info;
		}
	}

	info.Name = json["name"].GetString();
	info.AssetDir = json["assetsDir"].GetString();

	const Value& assembliesArray = json["assemblies"];
	if(assembliesArray.IsArray())
	{
		for(SizeType i = 0; i < assembliesArray.Size(); i++)
		{
			if(assembliesArray[i].IsString())
				info.Assemblies.emplace_back(assembliesArray[i].GetString());
			else
				spdlog::warn("Project file 'assemblies' should contain only strings");
		}
	}
	else
		spdlog::warn("Project file 'assemblies' is not an array");

	return info;
}