#include <imgui.h>
#include <spdlog/spdlog.h>
#include <Yonai/Time.hpp>
#include <Yonai/Window.hpp>
#include <Yonai/Resource.hpp>
#include <YonaiEditor/Glue.hpp>
#include <YonaiEditor/EditorApp.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Graphics/Texture.hpp>
#include <Yonai/Components/FPSCamera.hpp>
#include <Yonai/Platform/FixDLLBoundaries.hpp>

// Systems //
#include <YonaiEditor/Systems/ImGUISystem.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>
#include <Yonai/Systems/Global/AudioSystem.hpp>
#include <Yonai/Systems/CameraControlSystem.hpp>

// Scripting //
#include <Yonai/Scripting/Class.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Components/ScriptComponent.hpp>

#include <Yonai/Audio/Sound.hpp>
#include <Yonai/Components/SoundSource.hpp>

using namespace std;
using namespace glm;
using namespace YonaiEditor;
using namespace Yonai;
using namespace Yonai::IO;
using namespace YonaiEditor::Systems;
using namespace Yonai::Systems;
using namespace Yonai::Graphics;
using namespace Yonai::Scripting;
using namespace Yonai::Components;

namespace fs = std::filesystem;

string ImGuiIniFilename = "";
string ProjectPathArg = "projectpath";
string AssembliesDirectory = "/Assets/Mono";
string AquaScriptEditorPath = AssembliesDirectory + "/YonaiScriptEditor.dll";

void EditorApp::Setup()
{
	Application::Setup();

	InitialiseScripting();

	// Add global systems
	SystemManager::Global()->Add<AudioSystem>();

	if (!HasArg("build"))
	{
		m_ImGUISystem = SystemManager::Global()->Add<ImGUISystem>(false);

		// Disable drawing to default framebuffer.
		// Instead store pointer to render system and call manually
		m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();
		m_RenderSystem->Enable(false);

		SystemManager::Global()->Add<SceneSystem>();
	}
	
	LaunchEditorService();
}

void EditorApp::Cleanup()
{
	Application::Cleanup();

	SystemManager::Global()->Remove<SceneSystem>();
	SystemManager::Global()->Remove<AudioSystem>();
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
	Assembly* assembly = ScriptEngine::LoadAssembly(GetExecutableDirectory().string() + AquaScriptEditorPath, true);
	MonoType* editorService = assembly->GetTypeFromClassName("YonaiEditor", "EditorService");

	// Let managed code add & remove native ImGUISystem
	assembly->BindManagedSystem<Systems::ImGUISystem>("YonaiEditor.Systems", "ImGUISystem");

	SystemManager::Global()->Add(editorService);
}

void EditorApp::InitialiseScripting()
{
	ScriptEngine::Init(GetExecutableDirectory().string() + AssembliesDirectory,
		// Allow debugging in debug builds
		true);

	// Add AquaScriptEditor internal methods
	ScriptEngine::AddInternalCalls(_InternalMethods);
}

void EditorApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_ImGUISystem)
		m_ImGUISystem->PrepareDraw();
	
	SystemManager::Global()->Draw();

	Window::SwapBuffers();
	Window::PollEvents();

	Time::OnFrameEnd();
}
