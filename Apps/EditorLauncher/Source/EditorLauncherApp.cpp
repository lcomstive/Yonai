#include <imgui.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <portable-file-dialogs.h>
#include <AquaEngine/SystemManager.hpp>
#include "../Include/EditorLauncherApp.hpp"
#include <AquaEngine/Platform/FixDLLBoundaries.hpp>
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>

#include <AquaEngine/Graphics/RenderPipeline.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEditorLauncher;

namespace fs = std::filesystem;

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
}

#include <imgui.h>
void EditorLauncherApp::OnDraw()
{ }

bool EditorLauncherApp::LaunchEditor()
{
	/*
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

	if (!fs::exists(fmt::format("{}/Scripting/{}.csproj", project.Directory, project.Name)))
		CreateCSharpProject(project.Directory, project.Name);

	// Create new process of AquaEditor
#if defined(AQUA_PLATFORM_WINDOWS)
	// additional information
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;

	// set the size of the structures
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory(&processInfo, sizeof(processInfo));

	// start the program up
	if (!CreateProcess(
		editorPath.string().c_str(),	// Executable path
		(char*)args.c_str(),			// Command line args
		NULL,							// Process handle not inheritable
		NULL,							// Thread handle not inheritable
		FALSE,							// Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,				// creation flags
		NULL,							// Use parent's environment block
		NULL,							// Use parent's starting directory 
		&startupInfo,					// Pointer to STARTUPINFO structure
		&processInfo					// Pointer to PROCESS_INFORMATION structure
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
	if (!fs::exists(editorPath))
	{
		spdlog::error("Failed to launch editor - could not find executable");
		return false;
	}

	if (execl(editorPath.c_str(), editorPath.c_str(), args.c_str(), nullptr))
	{
		spdlog::error("[{}] Failed to launch editor", errno);
		return false;
	}

	spdlog::debug("Successfully launcher editor");
#endif

	// If editor is opening and setting enabled, close the launcher
	if (m_Settings.CloseLaucherOnEditorOpen)
		Exit();
	*/

	return true;
}

void EditorLauncherApp::AddProjectPrompt()
{
	string folderPathRaw = pfd::select_folder("Aqua Project Location").result();
	if (folderPathRaw.empty())
		return;

	fs::path folderPath(folderPathRaw);
	fs::path projectPath = folderPath / "project.json";

	if (fs::exists(projectPath))
		AddExistingProject(projectPath);
	else
		AddNewProject(folderPath);
}

void EditorLauncherApp::AddExistingProject(fs::path projectInfoPath)
{ }

