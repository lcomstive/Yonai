#include <imgui.h>
#include <EditorApp.hpp>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Components/FPSCamera.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/CameraControlSystem.hpp>

// Views //
#include <Views/Stats.hpp>
#include <Views/Viewport.hpp>

// Scripting //
#include <AquaEngine/Scripting/Class.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/ScriptComponent.hpp>

using namespace std;
using namespace glm;
using namespace AquaEditor;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

string ProjectPathArg = "ProjectPath";
string CSharpDLLPath = "dll-path";
string CSharpCoreDLLPath = "core-dll-path";

void EditorApp::Setup()
{
	WindowedApplication::Setup();

	Window::SetTitle("Aqua Editor");

	m_ProjectPath = GetArg(ProjectPathArg, "./");
	if (m_ProjectPath.empty())
		spdlog::warn("Empty project path!");
	if (m_ProjectPath[m_ProjectPath.size() - 1] != '/')
		m_ProjectPath += '/';
	spdlog::info("Project path: {}", m_ProjectPath);

	VFS::Mount("/Project", m_ProjectPath);
	VFS::Mount("/Assets", m_ProjectPath + "Assets");
	// VFS::Mount("/EditorCache", m_ProjectPath + ".aqua");

	InitialiseScripting();

	// Disable drawing to default framebuffer.
	// Instead store pointer to render system and call manually
	m_RenderSystem = SystemManager::Global()->Get<RenderSystem>();
	m_RenderSystem->Enable(false);

	LoadScene();

	Add<ViewportView>();
}

void EditorApp::OnDraw()
{
	DrawUI();
}

void EditorApp::OnUpdate()
{
	// Iterate over & update views
	for (auto& viewPair : m_Views)
		viewPair.second->Update();

	if(ScriptEngine::AwaitingReload())
		ScriptEngine::Reload();
}

void EditorApp::LoadScene()
{
	m_CurrentScene = new World("Test World");

	// Add a camera
	/*
	Entity cameraEntity = m_CurrentScene->CreateEntity();
	Transform* cameraTransform = cameraEntity.AddComponent<Transform>();
	cameraTransform->Position = { 0, 0, -10 };
	m_EditorCamera = cameraEntity.AddComponent<Camera>();
	m_EditorCamera->Orthographic = false;
	cameraEntity.AddComponent<FPSCamera>();
	*/

	// 2D Sprite Test
	ResourceID textureID = Resource::Load<Texture>("Texture/Test", "/Assets/Textures/Test.png");
	ResourceID spriteShader = Resource::Load<Shader>("Shaders/Sprite", ShaderStageInfo
		{
			"/Assets/Shaders/Sprite.vert",
			"/Assets/Shaders/Sprite.frag"
		});

	const unsigned int spriteRows = 20;
	const unsigned int spriteColumns = 20;
	for (unsigned int x = 0; x < spriteRows; x++)
	{
		for (unsigned int y = 0; y < spriteColumns; y++)
		{
			Entity entity = m_CurrentScene->CreateEntity();

			Transform* transform = entity.AddComponent<Transform>();
			transform->Position = { x - (spriteRows / 2.0f), y - (spriteColumns / 2.0f), 0 };
			transform->Scale = { 0.15f, 0.15f, 0.15f };

			SpriteRenderer* sprite = entity.AddComponent<SpriteRenderer>();
			sprite->Sprite = textureID;
			sprite->Shader = spriteShader;
		}
	}

	// Add managed component to first entity
	Assembly* assembly = ScriptEngine::GetAssemblies()[1];
	MonoType* monoType = assembly->GetTypeFromClassName("ScriptingTest", "TestComponent");
	for (int i = 0; i < spriteRows * spriteColumns; i++)
	{
		if (monoType)
			m_CurrentScene->GetEntity(i).AddComponent(monoType);
		else
			spdlog::error("Could not find type matching 'ScriptingTest.TestComponent'");
	}

	// Add scene to active scenes
	SceneSystem::UnloadAllScenes();
	SceneSystem::AddScene(m_CurrentScene);
}

void EditorApp::InitialiseScripting()
{
	string coreDllPath = GetArg(CSharpCoreDLLPath, "/Assets/Scripts/AquaScriptCore.dll");
	if (coreDllPath.empty() || !VFS::Exists(coreDllPath))
	{
		spdlog::critical("Core DLL path not specified or invalid. Set the '-core-dll-path' flag to 'AquaScriptCore.dll'");
		Exit();
		return;
	}
	ScriptEngine::Init(coreDllPath);

	string assemblyPath = GetArg(CSharpDLLPath, "/Assets/Scripts/TestGame.dll");
	if (assemblyPath.empty())
	{
		spdlog::warn("No C# DLL path found, scripting engine disabled");
		spdlog::warn("C# DLL can be set using the '-dll-path' flag");
	}
	else
		Assembly* assembly = ScriptEngine::LoadAssembly(VFS::GetAbsolutePath(assemblyPath), true);
}

void EditorApp::DrawUI()
{
	// Note: Switch this to true to enable dockspace
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1

			if(ImGui::MenuItem("Reload Scripts"))
				ScriptEngine::Reload(true);

			if (ImGui::MenuItem("Exit")) Exit();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Viewport"))
				Add<ViewportView>()->Open();

			if (ImGui::MenuItem("Stats"))
				Add<StatsView>()->Open();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// Iterate over & draw views
	for (auto& viewPair : m_Views)
		viewPair.second->Draw();

	ImGui::End();
}