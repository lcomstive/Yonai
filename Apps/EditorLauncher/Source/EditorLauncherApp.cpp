#include <imgui.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <portable-file-dialogs.h>
#include <AquaEngine/SystemManager.hpp>
#include "../Include/EditorLauncherApp.hpp"
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>
#include <AquaEngine/Platform/NativeFileBrowser.hpp>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <AquaEngine/Graphics/RenderPipeline.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

#if defined(AQUA_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(AQUA_PLATFORM_MAC) || defined(AQUA_PLATFORM_LINUX)
#include <unistd.h>
#endif

using namespace std;
using namespace rapidjson;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEditorLauncher;

namespace fs = std::filesystem;

struct LauncherSettings
{
	bool CloseLaucherOnEditorOpen = false;
};

const char* LauncherSettingsPath = "data://Launcher.json";

LauncherSettings Settings = {};
vector<ProjectInfo> Projects = {};

void EditorLauncherApp::Setup()
{
	WindowedApplication::Setup();

	Window::SetTitle("Aqua Editor");

	SystemManager::Global()->Remove<Systems::RenderSystem>();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (VFS::Exists(LauncherSettingsPath))
		ReadSettings();
	else
		WriteSettings();
}

#include <imgui.h>
void EditorLauncherApp::OnDraw()
{
	// Get ImGUI window to use the entire viewport
	#ifdef IMGUI_HAS_VIEWPORT
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	#else 
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	#endif
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::Begin("AquaEditor Launcher", nullptr,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

	// Menu bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1

			if (ImGui::MenuItem("Exit")) Exit();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// Add project
	if (ImGui::Button("Add Project"))
		AddProjectPrompt();

	ImGui::BeginTable("Projects", 3, ImGuiTableFlags_RowBg);

	ImGui::TableSetupColumn("Name", 	ImGuiTableColumnFlags_WidthStretch);
	ImGui::TableSetupColumn("Open", 	ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableSetupColumn("Remove", 	ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableHeadersRow();

	ImVec4 rowBg = { 1, 1, 1, 0.25f };
	ImVec4 rowBgAlt = { 1, 1, 1, 0.1f };
	ImGui::PushStyleColor(ImGuiCol_TableRowBg, rowBg);
	ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, rowBgAlt);

	// Existing Projects
	for(size_t i = 0; i < Projects.size(); i++)
	{
		ProjectInfo info = Projects[i];

		ImGui::TableNextRow(ImGuiTableRowFlags_None);
		ImGui::TableNextColumn();

		ImGui::Text(info.Name.c_str());

		ImColor subtextColor = { 1.0f, 1.0f, 1.0f, 0.5f };
		ImGui::TextColored(subtextColor, info.Path.c_str());

		ImGui::TableNextColumn();

		if(ImGui::Button(("->##" + info.Directory).c_str()))
			LaunchEditor(info);

		ImGui::TableNextColumn();
		if (ImGui::Button(("X##" + info.Directory).c_str()))
			RemoveProject(info);
	}
	ImGui::PopStyleColor(2);
	ImGui::EndTable();

	ImGui::End();
	ImGui::PopStyleVar();
}

bool EditorLauncherApp::LaunchEditor(ProjectInfo& project)
{
	// Get path to editor
	fs::path& launcherPath = GetExecutablePath();
	fs::path editorPath = launcherPath.parent_path();

#if defined(AQUA_PLATFORM_WINDOWS)
	editorPath.append("Aqua Editor.exe");
#elif defined(AQUA_PLATFORM_APPLE)
	editorPath.append("Aqua Editor");
#else
	editorPath.append("AquaEditor");
#endif

	std::string args = "-ProjectPath=\"" + project.Path + "\"";

	spdlog::debug("Launching editor - '{} {}'", editorPath.string().c_str(), args.c_str());

	if(!fs::exists(fs::path(project.Path) / ("Scripting/" + project.Name + ".vcproj")))
		CreateCSharpProject(project.Directory, project.Name);

	// Create new process of AquaEditor
#if defined(AQUA_PLATFORM_WINDOWS)
	// TODO: TEST ON WINDOWS

	// additional information
	STARTUPINFO startupInfo;     
	PROCESS_INFORMATION processInfo;

	// set the size of the structures
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory(&processInfo, sizeof(processInfo));

	// start the program up
	if(!CreateProcess(
		editorPath.string().c_str(),   // the path
		(char*)args.c_str(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&startupInfo,   // Pointer to STARTUPINFO structure
		&processInfo    // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	))
	{
		LPSTR messageBuffer = nullptr;

		//Ask Win32 to give us the string version of that message ID.
		//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		//Copy the error message into a std::string.
		std::string errMessage(messageBuffer, size);

		//Free the Win32's string's buffer.
		LocalFree(messageBuffer);

		spdlog::error("Failed to launch editor - {}", errMessage);
		return false;
	}

	spdlog::debug("Successfully launcher editor");
#elif defined(AQUA_PLATFORM_MAC) || defined(AQUA_PLATFORM_LINUX)
	if(!fs::exists(editorPath))
	{
		spdlog::error("Failed to launch editor - could not find executable");
		return false;
	}

	if(execl(editorPath.c_str(), editorPath.c_str(), args.c_str(), nullptr))
	{
		spdlog::error("[{}] Failed to launch editor", errno);
		return false;
	}

	spdlog::debug("Successfully launcher editor");
#endif

	// If editor is opening and setting enabled, close the launcher
	if (Settings.CloseLaucherOnEditorOpen)
		Exit();
	
	return true;
}

void EditorLauncherApp::RemoveProject(ProjectInfo& project)
{
	spdlog::debug("Removing reference to project '{}'", project.Name.c_str());
	spdlog::debug("Path: {}", project.Path.c_str());
	
	for (auto it = Projects.rbegin(); it != Projects.rend(); it++)
	{
		if (project.Path.compare(it->Path) != 0)
			continue; // Not this one

		// Remove project
		Projects.erase(--it.base());
		break;
	}
	WriteSettings();
}

void EditorLauncherApp::ReadSettings()
{
	m_Settings.Parse(VFS::ReadText(LauncherSettingsPath).c_str());

	// Projects
	const Value& projectsArray = m_Settings["projects"];
	if (!projectsArray.IsNull() && projectsArray.IsArray())
	{
		for (SizeType i = 0; i < projectsArray.Size(); i++)
		{
			// Make sure proper values are present
			if (!projectsArray[i].HasMember("name") ||
				!projectsArray[i].HasMember("path") ||
				!projectsArray[i].HasMember("directory"))
				continue;

			// Add to list of available projects
			Projects.emplace_back(ProjectInfo{
				projectsArray[i]["name"].GetString(),
				projectsArray[i]["path"].GetString(),
				projectsArray[i]["directory"].GetString()
				});
		}
	}
	else
		m_Settings["projects"].SetArray();

	if (!m_Settings.HasMember("settings"))
		m_Settings["settings"] = Value();

	// settings.closeLauncherOnEditorOpen
	if (m_Settings["settings"].HasMember("closeLauncherOnEditorOpen"))
			Settings.CloseLaucherOnEditorOpen = m_Settings["settings"]["closeLauncherOnEditorOpen"].GetBool();
	else
		m_Settings["settings"]["closeLauncherOnEditorOpen"].SetBool(Settings.CloseLaucherOnEditorOpen);
}

void EditorLauncherApp::WriteSettings()
{
	m_Settings.SetObject();

	// Create projects array
	Value projectsArray(kArrayType);
	Document::AllocatorType& allocator = m_Settings.GetAllocator();

	for (int i = 0; i < Projects.size(); i++)
	{
		Value objValue(kObjectType);
		objValue.AddMember("name", Value(Projects[i].Name.c_str(), allocator), allocator);
		objValue.AddMember("path", Value(Projects[i].Path.c_str(), allocator), allocator);
		objValue.AddMember("directory", Value(Projects[i].Directory.c_str(), allocator), allocator);

		projectsArray.PushBack(objValue, allocator);
	}
	m_Settings.AddMember("projects", projectsArray, allocator);

	// Create settings object
	Value settings(kObjectType);
	settings.AddMember("closeLauncherOnEditorOpen", Settings.CloseLaucherOnEditorOpen, allocator);

	m_Settings.AddMember("settings", settings, allocator);

	// Write data to file
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	m_Settings.Accept(writer);

	VFS::WriteText(LauncherSettingsPath, buffer.GetString());
}

void EditorLauncherApp::AddProjectPrompt()
{
	string folderPathRaw = pfd::select_folder("Aqua Project Location").result();
	if(folderPathRaw.empty())
		return;

	fs::path folderPath(folderPathRaw);
	fs::path projectPath = folderPath / "project.json";

	if (fs::exists(projectPath))
		AddExistingProject(projectPath);
	else
		AddNewProject(folderPath);		
}

void EditorLauncherApp::AddExistingProject(fs::path projectInfoPath)
{
	string projectInfoStr = projectInfoPath.string();
	replace(projectInfoStr.begin(), projectInfoStr.end(), '\\', '/');

	spdlog::info("Adding existing project at path '{}'", projectInfoStr.c_str());

	Document projectInfo;
	string json = VFS::ReadText("file://" + projectInfoStr);
	projectInfo.Parse(json.c_str());

	string projectName = projectInfoPath.parent_path().filename().string();
	if (projectInfo.HasMember("name"))
		projectName = projectInfo["name"].GetString();

	fs::path projectDir = projectInfoPath.parent_path();
	if(!fs::exists(projectDir / ("Scripting/" + projectName + ".vcproj")))
		CreateCSharpProject(projectDir, projectName);

	Projects.emplace_back(ProjectInfo
		{
			projectName,
			projectInfoStr,
			projectDir.string()
		});
	WriteSettings();
}

// Based off https://stackoverflow.com/a/3418285
void AquaEditorLauncher::ReplaceAll(string& input, const string& from, const string& to)
{
	if (from.empty())
		return;

	size_t startPos = 0;
	while ((startPos = input.find(from, startPos)) != string::npos)
	{
		input.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

void ReplaceTextInNewProjectFile(string path, string projectName, string editorDir)
{
	string contents = VFS::ReadText(path);

	ReplaceAll(contents, "$<ProjectName>", projectName);
	ReplaceAll(contents, "$<EditorDir>", editorDir);

	VFS::WriteText(path, contents);
}

void EditorLauncherApp::AddNewProject(fs::path projectDir)
{
	spdlog::info("Adding new project at path '{}'", projectDir.string().c_str());

	VFS::Copy("app://Assets/ProjectTemplate/", "file://" + projectDir.string());
	fs::path projectInfo = projectDir / "project.json";

	string projectName = projectDir.filename().string();

#if defined(AQUA_PLATFORM_APPLE)
	string editorDir = (GetExecutablePath().parent_path().parent_path() / "Resources").string();
#else
	string editorDir = GetExecutablePath().parent_path().string();
#endif

	ReplaceTextInNewProjectFile("file://" + projectDir.string() + "/project.json", projectName, editorDir);

	CreateCSharpProject(projectDir, projectName);

	Projects.emplace_back(ProjectInfo
		{
			projectName,
			projectInfo.string(),
			projectDir.string()
		});
	WriteSettings();
}
