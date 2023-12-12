#include <imgui.h>
#include <spdlog/spdlog.h>
#include <Yonai/Time.hpp>
#include <Yonai/Window.hpp>
#include <Yonai/Resource.hpp>
#include <YonaiEditor/Glue.hpp>
#include <YonaiEditor/EditorApp.hpp>
#include <Yonai/Platform/FixDLLBoundaries.hpp>

// Systems //
#include <Yonai/Systems/Global/SceneSystem.hpp>
#include <Yonai/Systems/Global/AudioSystem.hpp>

// Scripting //
#include <Yonai/Scripting/Class.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Components/ScriptComponent.hpp>

using namespace std;
using namespace glm;
using namespace YonaiEditor;
using namespace Yonai;
using namespace Yonai::IO;
using namespace Yonai::Systems;
using namespace Yonai::Scripting;
using namespace Yonai::Components;

namespace fs = std::filesystem;

string ImGuiIniFilename = "";
string ProjectPathArg = "projectpath";
string AssembliesDirectory = "/Assets/Mono";
string YonaiScriptEditorPath = AssembliesDirectory + "/YonaiScriptEditor.dll";

void EditorApp::Setup()
{
	Application::Setup();

	InitialiseScripting();

	// Add global systems
	SystemManager::Global()->Add<AudioSystem>();

	if (!HasArg("build"))
		SystemManager::Global()->Add<SceneSystem>();
		
	LaunchEditorService();
}

void EditorApp::Cleanup()
{
	Application::Cleanup();

	SystemManager::Global()->Remove<SceneSystem>();
	SystemManager::Global()->Remove<AudioSystem>();
}

void EditorApp::OnUpdate()
{
	if(ScriptEngine::AwaitingReload())
		ScriptEngine::Reload();

	Draw();
}

void EditorApp::LaunchEditorService()
{
	Assembly* assembly = ScriptEngine::LoadAssembly(GetExecutableDirectory().string() + YonaiScriptEditorPath
		, true);
	MonoType* editorService = assembly->GetTypeFromClassName("YonaiEditor", "EditorService");

	SystemManager::Global()->Add(editorService);
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
	SystemManager::Global()->Draw();

	Window::SwapBuffers();
	Window::PollEvents();

	Time::OnFrameEnd();
}
