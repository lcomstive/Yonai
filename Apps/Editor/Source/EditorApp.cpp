#include <imgui.h>
#include <spdlog/spdlog.h>
#include <AquaEditor/Glue.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEditor/EditorApp.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Components/FPSCamera.hpp>
#include <AquaEngine/Platform/FixDLLBoundaries.hpp>

// Systems //
#include <AquaEditor/Systems/ImGUISystem.hpp>
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
using namespace AquaEditor::Systems;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

namespace fs = std::filesystem;

string ImGuiIniFilename = "";
string ProjectPathArg = "projectpath";
string AssembliesDirectory = "./Assets/Mono/";
string AquaScriptEditorPath = AssembliesDirectory + "AquaScriptEditor.dll";

void EditorApp::Setup()
{
	Application::Setup();

	if (!HasArg(ProjectPathArg))
	{
		spdlog::warn("No project path set, use '-{} <path>' argument", ProjectPathArg);
		Exit();
		return;
	}

	InitialiseMounts();
	InitialiseScripting();

	// Add global systems
	SystemManager::Global()->Add<AudioSystem>();

	ImGUISystem* imGUISystem = SystemManager::Global()->Add<ImGUISystem>();
	ImGui::SetCurrentContext(imGUISystem->GetContext());

	ImGui::GetIO().IniFilename = "./EditorLayout.ini";

	// Disable drawing to default framebuffer.
	// Instead store pointer to render system and call manually
	m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();
	m_RenderSystem->Enable(false);

	SystemManager::Global()->Add<SceneSystem>();
	
	LaunchEditorService();
}

void EditorApp::Cleanup()
{
	Application::Cleanup();

	SystemManager::Global()->Remove<SceneSystem>();
	SystemManager::Global()->Remove<AudioSystem>();
	SystemManager::Global()->Remove<ImGUISystem>();
	SystemManager::Global()->Remove<RenderSystem>();

	m_RenderSystem = nullptr;
}

void EditorApp::OnUpdate()
{
	if(ScriptEngine::AwaitingReload())
		ScriptEngine::Reload();

	Draw();
}

void EditorApp::LaunchEditorService()
{
	Assembly* assembly = ScriptEngine::LoadAssembly(AquaScriptEditorPath, true);
	MonoType* editorService = assembly->GetTypeFromClassName("AquaEditor", "EditorService");

	// Let managed code add & remove native ImGUISystem
	assembly->BindManagedSystem<Systems::ImGUISystem>("AquaEditor.Systems", "ImGUISystem");

	SystemManager::Global()->Add(editorService);
}

void EditorApp::InitialiseScripting()
{
	// VFS::Mount("mono://", "app://Assets/Mono");
	ScriptEngine::Init(AssembliesDirectory,
		// Allow debugging in debug builds
		true);

	// Add AquaScriptEditor internal methods
	ScriptEngine::AddInternalCalls(_InternalMethods);
}

void EditorApp::InitialiseMounts()
{
	if (!HasArg(ProjectPathArg))
	{
		spdlog::warn("No project path set, use '-{} <path>' argument", ProjectPathArg);
		Exit();
		return;
	}

	m_ProjectPath = fs::path(GetArg(ProjectPathArg));
	if (m_ProjectPath.empty())
		spdlog::warn("Empty project path!");
	spdlog::info("Project path: {}", m_ProjectPath.string().c_str());

	/*
	VFS::Mount("project://", m_ProjectPath.string());
	VFS::Mount("assets://", "project://Assets");
	VFS::Mount("assets://", "app://Assets"); // Default assets
	VFS::Mount("editor://", "project://.aqua");
	VFS::Mount("mono://", "app://Assets/Mono");
	*/
}

void EditorApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SystemManager::Global()->Draw();

	Window::SwapBuffers();
	Window::PollEvents();
}
