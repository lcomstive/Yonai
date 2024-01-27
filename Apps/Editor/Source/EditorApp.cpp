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

string AssembliesDirectory = "/Assets/Mono";
string YonaiScriptEditorPath = AssembliesDirectory + "/YonaiScriptEditor.dll";
SystemManager EditorApp::m_SystemManager = SystemManager(nullptr);

EditorApp::EditorApp() : Yonai::Application() { }
EditorApp::EditorApp(int argc, char** argv) : Yonai::Application(argc, argv) { }

void EditorApp::Setup()
{
	Application::Setup();

	InitialiseScripting();

	// Add global systems
	SystemManager::Global()->ShouldUpdate = false;
	SystemManager::Global()->Add<AudioSystem>();

	if (!HasArg("build"))
	{
		m_SystemManager.Add<ImGUISystem>(false /* Enable from C# side after setting INI filename and other settings */);
		SystemManager::Global()->Add<SceneSystem>();
	}
	
	LaunchEditorService();

	m_SystemManager.Enable(true);
}

void EditorApp::Cleanup()
{
	Application::Cleanup();

	m_SystemManager.Remove<ImGUISystem>();
	SystemManager::Global()->Remove<SceneSystem>();
	SystemManager::Global()->Remove<AudioSystem>();
}

void EditorApp::OnUpdate()
{
	if(ScriptEngine::AwaitingReload())
		ScriptEngine::Reload();

	m_SystemManager.Update();

	Draw();
}

void EditorApp::LaunchEditorService()
{
	Assembly* assembly = ScriptEngine::LoadAssembly(GetExecutableDirectory().string() + YonaiScriptEditorPath
		, true);
	MonoType* editorService = assembly->GetTypeFromClassName("YonaiEditor", "EditorService");

	// Let managed code add & remove native ImGUISystem
	assembly->BindManagedSystem<Systems::ImGUISystem>("YonaiEditor.Systems", "ImGUISystem");

	m_SystemManager.Add(editorService);
}

void EditorApp::InitialiseScripting()
{
	ScriptEngine::Init(GetExecutableDirectory().string() + AssembliesDirectory,
		// Allow debugging in debug builds
		true);

	// Add YonaiScriptEditor internal methods
	ScriptEngine::AddInternalCalls(_InternalMethods);
}

void EditorApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	SystemManager::Global()->Draw();
	m_SystemManager.Draw();

	Window::SwapBuffers();
	Window::PollEvents();
}

SystemManager& EditorApp::GetSystemManager() { return m_SystemManager; }