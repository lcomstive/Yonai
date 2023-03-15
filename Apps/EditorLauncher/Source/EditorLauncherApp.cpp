#include <imgui.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <portable-file-dialogs.h>
#include <AquaEngine/SystemManager.hpp>
#include "../Include/EditorLauncherApp.hpp"
#include <AquaEngine/Platform/FixDLLBoundaries.hpp>
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <AquaEngine/Graphics/RenderPipeline.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

using namespace std;
using namespace rapidjson;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEditorLauncher;

namespace fs = std::filesystem;

const char* LauncherSettingsPath = "data://Launcher.json";

void EditorLauncherApp::Setup()
{
	WindowedApplication::Setup();

	// FIX_DLL_BOUNDARIES();
	ImGui::SetCurrentContext(SystemManager::Global()->Get<Systems::ImGUISystem>()->GetContext());
	
	glfwMakeContextCurrent(Window::GetNativeHandle());
	int result = gladLoadGL(glfwGetProcAddress);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Window::SetTitle("Aqua Editor");

	SystemManager::Global()->Remove<Systems::RenderSystem>();

	VFS::Mount("assets://", "app://Assets/");

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

	ImGui::BeginTable("m_Projects", 3, ImGuiTableFlags_RowBg);

	ImGui::TableSetupColumn("Name", 	ImGuiTableColumnFlags_WidthStretch);
	ImGui::TableSetupColumn("Open", 	ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableSetupColumn("Remove", 	ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableHeadersRow();

	ImVec4 rowBg = { 1, 1, 1, 0.25f };
	ImVec4 rowBgAlt = { 1, 1, 1, 0.1f };
	ImGui::PushStyleColor(ImGuiCol_TableRowBg, rowBg);
	ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, rowBgAlt);

	// Existing m_Projects
	for(size_t i = 0; i < m_Projects.size(); i++)
	{
		ProjectInfo info = m_Projects[i];

		ImGui::TableNextRow(ImGuiTableRowFlags_None);
		ImGui::TableNextColumn();

		ImGui::Text("%s", info.Name.c_str());

		ImColor subtextColor = { 1.0f, 1.0f, 1.0f, 0.5f };
		ImGui::TextColored(subtextColor, "%s", info.Path.c_str());

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

	std::string args = "-ProjectPath=\"" + project.Directory + "\"";

	spdlog::debug("Launching editor - '{} {}'", editorPath.string().c_str(), args.c_str());

	if(!fs::exists(fmt::format("{}/Scripting/{}.csproj", project.Directory, project.Name)))
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
	if (m_Settings.CloseLaucherOnEditorOpen)
		Exit();
	
	return true;
}

void EditorLauncherApp::RemoveProject(ProjectInfo& project)
{
	spdlog::debug("Removing reference to project '{}'", project.Name.c_str());
	spdlog::debug("Path: {}", project.Path.c_str());
	
	for (auto it = m_Projects.rbegin(); it != m_Projects.rend(); it++)
	{
		if (project.Path.compare(it->Path) != 0)
			continue; // Not this one

		// Remove project
		m_Projects.erase(--it.base());
		break;
	}
	WriteSettings();
}

void EditorLauncherApp::ReadSettings()
{
	m_SettingsJSON.Parse(VFS::ReadText(LauncherSettingsPath).c_str());

	// m_Projects
	const Value& m_ProjectsArray = m_SettingsJSON["projects"];
	if (!m_ProjectsArray.IsNull() && m_ProjectsArray.IsArray())
	{
		for (SizeType i = 0; i < m_ProjectsArray.Size(); i++)
		{
			// Make sure proper values are present
			if (!m_ProjectsArray[i].HasMember("name") ||
				!m_ProjectsArray[i].HasMember("path") ||
				!m_ProjectsArray[i].HasMember("directory"))
				continue;

			// Add to list of available m_Projects
			m_Projects.emplace_back(ProjectInfo{
				m_ProjectsArray[i]["name"].GetString(),
				m_ProjectsArray[i]["path"].GetString(),
				m_ProjectsArray[i]["directory"].GetString()
				});
		}
	}
	else
		m_SettingsJSON["m_Projects"].SetArray();

	if (!m_SettingsJSON.HasMember("settings"))
		m_SettingsJSON["settings"] = Value();

	// settings.closeLauncherOnEditorOpen
	if (m_SettingsJSON["settings"].HasMember("closeLauncherOnEditorOpen"))
			m_Settings.CloseLaucherOnEditorOpen = m_SettingsJSON["settings"]["closeLauncherOnEditorOpen"].GetBool();
	else
		m_SettingsJSON["settings"]["closeLauncherOnEditorOpen"].SetBool(m_Settings.CloseLaucherOnEditorOpen);
}

void EditorLauncherApp::WriteSettings()
{
	m_SettingsJSON.SetObject();

	// Create m_Projects array
	Value m_ProjectsArray(kArrayType);
	Document::AllocatorType& allocator = m_SettingsJSON.GetAllocator();

	for (int i = 0; i < m_Projects.size(); i++)
	{
		Value objValue(kObjectType);
		objValue.AddMember("name", Value(m_Projects[i].Name.c_str(), allocator), allocator);
		objValue.AddMember("path", Value(m_Projects[i].Path.c_str(), allocator), allocator);
		objValue.AddMember("directory", Value(m_Projects[i].Directory.c_str(), allocator), allocator);

		m_ProjectsArray.PushBack(objValue, allocator);
	}
	m_SettingsJSON.AddMember("projects", m_ProjectsArray, allocator);

	// Create settings object
	Value settings(kObjectType);
	settings.AddMember("closeLauncherOnEditorOpen", m_Settings.CloseLaucherOnEditorOpen, allocator);

	m_SettingsJSON.AddMember("settings", settings, allocator);

	// Write data to file
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	m_SettingsJSON.Accept(writer);

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

	m_Projects.emplace_back(ProjectInfo
		{
			projectName,
			projectInfoStr,
			projectDir.string()
		});
	WriteSettings();
}

