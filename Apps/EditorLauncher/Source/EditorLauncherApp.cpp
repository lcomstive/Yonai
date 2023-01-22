#include <imgui.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/SystemManager.hpp>
#include "../Include/EditorLauncherApp.hpp"
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>

#include <AquaEngine/Graphics/RenderPipeline.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

#if defined(AQUA_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(AQUA_PLATFORM_MAC) || defined(AQUA_PLATFORM_LINUX)
#include <unistd.h>
#endif

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEditorLauncher;

namespace fs = std::filesystem;

struct ProjectInfo
{
	string Name;
	string Path;
};

vector<ProjectInfo> TestProjects = {
	{ "TestApp", "/Users/lcomstive/Projects/AquaEngine/Apps/TestGame" },
	{ "Another One", "/Users/lcomstive/Projects/AquaEngine/Apps/AnotherOne" },
	{ "Game Project", "/Users/lcomstive/Projects/AquaEngine/Apps/GameProject" },
	{ "Tutorials", "/Users/lcomstive/Projects/AquaEngine/Apps/Tutorials" }
};

void EditorLauncherApp::Setup()
{
	WindowedApplication::Setup();

	Window::SetTitle("Aqua Editor");

	SystemManager::Global()->Remove<Systems::RenderSystem>();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	for(ProjectInfo& info : TestProjects)
	{
		ImGui::TableNextRow(ImGuiTableRowFlags_None);
		ImGui::TableNextColumn();

		ImGui::Text(info.Name.c_str());

		ImColor subtextColor = { 1.0f, 1.0f, 1.0f, 0.5f };
		ImGui::TextColored(subtextColor, info.Path.c_str());

		ImGui::TableNextColumn();

		if(ImGui::Button(("->##" + info.Path).c_str()))
			LaunchEditor(info.Path);
		// { spdlog::debug("Opening project at path '{}'", info.Path.c_str()); }

		ImGui::TableNextColumn();
		if(ImGui::Button(("X##" + info.Path).c_str()))  { spdlog::debug("Removing project at path '{}'", info.Path.c_str()); }
	}
	ImGui::PopStyleColor(2);
	ImGui::EndTable();

	ImGui::End();
	ImGui::PopStyleVar();
}

bool EditorLauncherApp::LaunchEditor(std::string& projectPath)
{
	// Get path to editor
	fs::path& launcherPath = GetExecutablePath();
	fs::path editorPath = launcherPath.parent_path();
	editorPath.append("AquaEditor");

	std::string args = "-ProjectPath=" + projectPath;

	spdlog::debug("Launching editor - '{} {}'", editorPath.c_str(), args.c_str());

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
		editorPath.append(".exe").c_str(),   // the path
		argv[1],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&startupInfo,   // Pointer to STARTUPINFO structure
		&processIfno    // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	))
	{
		spdlog::error("Failed to launch editor - {}", GetLastError());
		return false;
	}

	spdlog::debug("Successfully launcher editor");

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

#elif defined(AQUA_PLATFORM_MAC) || defined(AQUA_PLATFORM_LINUX)
	if(!fs::exists(editorPath))
	{
		spdlog::error("Failed to launch editor - could not find executable");
		return false;
	}

	if(execl(editorPath.c_str(), editorPath.c_str(), args.c_str()))
	{
		spdlog::error("[{}] Failed to launch editor", errno);
		return false;
	}

	spdlog::debug("Successfully launcher editor");
#else
	// TODO: Implement launching editor process
#endif
	
	return true;
}