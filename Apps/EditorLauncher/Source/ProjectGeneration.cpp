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

// Gathers a list of files to add to the C# project, using files
// located in the given directory, skipping over any directories matching SourceSkippedDirectories.
// A string is returned containing all found source files formatted between XML valid for a .csproj file
string GetCompileItems(fs::path directory)
{
	// Get all CSharp files in project scripting directory
	vector<string> sourceFiles = {};
	fs::recursive_directory_iterator dirIt(directory), dirItEnd;
	while (dirIt != dirItEnd)
	{
		// Check if directory should be skipped
		for (const string& filter : SourceSkippedDirectories)
		{
			if (!dirIt->is_directory() || dirIt->path().filename().compare(filter) != 0)
				continue;

			// Skip over, disable recursing into this directory
			dirIt.disable_recursion_pending();
			break;
		}

		// Check for .cs extension
		const fs::path& path = dirIt->path();
		if (path.extension().compare(".cs") == 0)
			// Add to list of source files
			sourceFiles.emplace_back(fs::relative(path, directory).string());

		dirIt++;
	}

	// Build string for all sources file with format `<Compile Include="Class.cs" />`
	string compileItems = "<ItemGroup>\r\n";
	for (const string& source : sourceFiles)
		compileItems += "<Compile Include=\"" + source + "\" />\r\n";
	compileItems += "</ItemGroup>";

	return compileItems;
}

const string TemplateProjectDLLName = "AquaProject";
const string TemplateProjectPath = "AquaProject.csproj.template";
void EditorLauncherApp::CreateCSharpProject(fs::path projectPath, string projectName)
{
	projectPath /= "Scripting";
	string projectDir = projectPath.string();

	// Convert Windows path to Unix-like path for consistency
	replace(projectDir.begin(), projectDir.end(), '\\', '/');
	
	if(VFS::Exists(fmt::format("{}/{}.csproj", projectDir, projectName)))
		return; // No need to create project

	string vfsPath = fmt::format("file://{}/{}", projectDir, TemplateProjectPath);

	VFS::Copy(
		"assets://ProjectTemplate/Scripting/" + TemplateProjectPath,
		vfsPath
	);

	VFS::ReplaceText(vfsPath,
		{
			{ "$<ProjectName>", projectName },
			{ "$<EditorDir>",
		#if defined(AQUA_PLATFORM_APPLE)
				// Point to the app bundle's Resources/ folder
				(GetExecutablePath().parent_path().parent_path() / "Resources").string()
		#else
				GetExecutablePath().parent_path().string()
		#endif
			},
			{ "$<CompileItems>", GetCompileItems(projectPath) }
		});

	// Rename 'AquaTemplateProject.csproj.template' to 'ProjectName.csproj'
	VFS::Move(vfsPath, fmt::format("file://{}/{}.csproj", projectDir, projectName));

	// Rename the prebuilt .dll file to the project name
	VFS::Move(
		fmt::format("file://{}/bin/{}.dll", projectDir, TemplateProjectDLLName),
		fmt::format("file://{}/bin/{}.dll", projectDir, projectName)
		);
}

void EditorLauncherApp::AddNewProject(fs::path projectDir)
{
	spdlog::info("Adding new project at path '{}'", projectDir.string().c_str());

	// Copy template project directory to the desired project directory
	VFS::Copy("app://Assets/ProjectTemplate/", "file://" + projectDir.string());

	string projectInfo = (projectDir / "project.json").string();

	// Convert Windows path to unix-like path
	replace(projectInfo.begin(), projectInfo.end(), '\\', '/');

	// Default project name is the folder name
	string projectName = projectDir.filename().string();

	// Pairs of strings to replace in files
	vector<pair<string, string>> replacePairs =
	{
		{ "$<ProjectName>", projectName },
		{ "$<EditorDir>",
		#if defined(AQUA_PLATFORM_APPLE)
			(GetExecutablePath().parent_path().parent_path() / "Resources").string()
		#else
			GetExecutablePath().parent_path().string()
		#endif
		}
	};

	// Replace text in project.json file
	VFS::ReplaceText("file://" + projectInfo, replacePairs);

	// Create .csproj using C# files in Scripting/ subdirectory
	CreateCSharpProject(projectDir, projectName);
}