#include <string>
#include <vector>
#include <EditorLauncherApp.hpp>
#include <AquaEngine/IO/VFS.hpp>

using namespace std;
using namespace AquaEngine::IO;
using namespace AquaEditorLauncher;

namespace fs = std::filesystem;

const vector<string> SourceSkippedDirectories = 
{
	"obj",
	"bin"
};

const string TemplateProjectPath = "Scripting/AquaTemplateProject.csproj.template";
void EditorLauncherApp::CreateCSharpProject(fs::path projectDir, string projectName)
{
	string tempPath = projectDir.string() + "/" + TemplateProjectPath;
	string contents = VFS::ReadText("file://" + tempPath);

	projectDir /= "Scripting";

	// Replace project name and editor directory instances
	ReplaceAll(contents, "$<ProjectName>", projectName);

	ReplaceAll(contents, "$<EditorDir>", 
#if defined(AQUA_PLATFORM_APPLE)
		// Point to the app bundle's Resources/ folder
		(GetExecutablePath().parent_path().parent_path() / "Resources").string()
#else
		GetExecutablePath().parent_path().string()
#endif
	);

	vector<string> sourceFiles = {};
	// Get all CSharp files in project scripting directory
	fs::recursive_directory_iterator dirIt(projectDir), dirItEnd;
	while(dirIt != dirItEnd)
	{
		// Check if directory should be skipped
		for(const string& filter : SourceSkippedDirectories)
		{
			if(!dirIt->is_directory() || dirIt->path().filename().compare(filter) != 0)
				continue;

			// Skip over, disable recursing into this directory
			spdlog::debug("Skipping over '{}'", dirIt->path().string());
			dirIt.disable_recursion_pending();
			break;
		}

		// Check for .cs extension
		const fs::path& path = dirIt->path();
		if(path.extension().compare(".cs") == 0)
			// Add to list of source files
			sourceFiles.emplace_back(fs::relative(path, projectDir).string());

		dirIt++;
	}

	// Build string for all sources file with format `<Compile Include="Class.cs" />`
	string compileItems = "<ItemGroup>\r\n";
	for(const string& source : sourceFiles)
		compileItems += "<Compile Include=\"" + source + "\" />\r\n";
	compileItems += "</ItemGroup>";

	// Insert source files into project file
	ReplaceAll(contents, "$<CompileItems>", compileItems);

	VFS::WriteText("file://" + tempPath, contents);
	fs::rename(tempPath, projectDir.string() + "/" + projectName + ".csproj");
}