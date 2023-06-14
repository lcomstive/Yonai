#include <imgui.h>
#include <Glue.hpp>
#include <EditorApp.hpp>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Components/FPSCamera.hpp>
#include <AquaEngine/Platform/FixDLLBoundaries.hpp>

// Systems //
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>
#include <AquaEngine/Systems/CameraControlSystem.hpp>

// Scripting //
#include <AquaEngine/Scripting/Class.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/ScriptComponent.hpp>

#include <AquaEngine/Audio/Sound.hpp>
#include <AquaEngine/Components/SoundSource.hpp>

using namespace std;
using namespace glm;
using namespace AquaEditor;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

namespace fs = std::filesystem;

string ImGuiIniFilename = "";
string ProjectPathArg = "projectpath";
string AquaScriptCorePath = "app://AquaScriptCore.dll";

void EditorApp::Setup()
{
	Application::Setup();
	
	InitialiseScripting();

	// Add global systems
	SystemManager::Global()->Add<AudioSystem>();

	ImGUISystem* guiSystem = SystemManager::Global()->Add<ImGUISystem>();
	ImGui::SetCurrentContext(guiSystem->GetContext());

	// Disable drawing to default framebuffer.
	// Instead store pointer to render system and call manually
	m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();
	m_RenderSystem->Enable(false);

	SystemManager::Global()->Add<SceneSystem>();

	if(!HasArg(ProjectPathArg))
	{
		spdlog::warn("No project path set, use '-{} <path>' argument", ProjectPathArg);
		Exit();
		return;
	}
	
	m_ProjectPath = fs::path(GetArg(ProjectPathArg));
	if (m_ProjectPath.empty())
		spdlog::warn("Empty project path!");
	spdlog::info("Project path: {}", m_ProjectPath.string().c_str());

	string projectDir = m_ProjectPath.string();
	VFS::Mount("project://", projectDir);
	VFS::Mount("assets://", "app://Assets"); // Default assets
	VFS::Mount("assets://", "project://Assets");
	VFS::Mount("editor://", "project://.aqua");

	// Set ImGUI layout file
	ImGuiIO& io = ImGui::GetIO();
	ImGuiIniFilename = VFS::GetAbsolutePath("editor://EditorLayout.ini");
	io.IniFilename = ImGuiIniFilename.c_str();

	LaunchEditorService();
}

void EditorApp::Cleanup()
{
	Application::Cleanup();

	SystemManager::Global()->Remove<SceneSystem>();
	SystemManager::Global()->Remove<ImGUISystem>();
	SystemManager::Global()->Remove<AudioSystem>();
	SystemManager::Global()->Remove<RenderSystem>();

	m_RenderSystem = nullptr;
}

#include <AquaEngine/Input.hpp>
void EditorApp::OnUpdate()
{
	if(ScriptEngine::AwaitingReload())
		ScriptEngine::Reload();

	Draw();
}

void EditorApp::LaunchEditorService()
{
	Assembly* assembly = ScriptEngine::LoadAssembly("app://AquaScriptEditor.dll", true);
	MonoType* editorService = assembly->GetTypeFromClassName("AquaEditor", "EditorService");
	SystemManager::Global()->Add(editorService);
}

void EditorApp::InitialiseScripting()
{
	if (AquaScriptCorePath.empty() || !VFS::Exists(AquaScriptCorePath))
	{
		spdlog::critical("Core DLL path not specified or file '{}' does not exist.", AquaScriptCorePath.c_str());
		Exit();
		return;
	}

	ScriptEngine::Init(AquaScriptCorePath,
		// Allow debugging in debug builds
		true);

	// Add AquaScriptEditor internal methods
	ScriptEngine::AddInternalCalls(_InternalMethods);
}

void EditorApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SystemManager::Global()->Draw();

	Window::SwapBuffers();
	Window::PollEvents();
}
