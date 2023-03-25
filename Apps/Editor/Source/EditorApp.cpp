#include <imgui.h>
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
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>
#include <AquaEngine/Systems/CameraControlSystem.hpp>

// Views //
#include <Views/Stats.hpp>
#include <Views/Viewport.hpp>

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

vector<SoundSource*> soundSources = {};

void EditorApp::Setup()
{
	WindowedApplication::Setup();
	FIX_DLL_BOUNDARIES();

	Window::SetTitle("Aqua Editor");

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

	InitialiseScripting();

	LoadProject();

	// Disable drawing to default framebuffer.
	// Instead store pointer to render system and call manually
	m_RenderSystem = SystemManager::Global()->Get<RenderSystem>();
	m_RenderSystem->Enable(false);

	SystemManager::Global()->Add<AudioSystem>();

	Add<ViewportView>();

	LoadScene();
}

void EditorApp::OnDraw()
{
	DrawUI();
}

#include <AquaEngine/Input.hpp>
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
	ResourceID textureID = Resource::Load<Texture>("Texture/Test", "assets://Textures/Test.png");
	ResourceID spriteShader = Resource::Load<Shader>("Shaders/Sprite", ShaderStageInfo
		{
			"assets://Shaders/Sprite.vert",
			"assets://Shaders/Sprite.frag"
		});

	// Test C# component
	auto assemblies = ScriptEngine::GetAssemblies();
	if(assemblies.size() <= 1)
		return;
	Assembly* assembly = assemblies[1];
	MonoType* monoType = assembly->GetTypeFromClassName("TestGame", "TestComponent");

	/*
	const unsigned int spriteRows = 15;
	const unsigned int spriteColumns = 15;
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

			if (x % 2 == 0 && y % 2 == 0)
				entity.AddComponent(monoType);
		}
	}
	*/

	// Implemented in C#
	MonoType* testSystem = assembly->GetTypeFromClassName("TestGame", "TestSystem");
	if(testSystem)
		m_CurrentScene->GetSystemManager()->Add(testSystem);

	/// TEMP ///
	ResourceID bellSoundID = Resource::Load<Sound>("Sounds/Bell", "assets://Audio/Bell.mp3");
	ResourceID musicSoundID = Resource::Load<Sound>("Sounds/Music", "assets://Audio/Lifelike.mp3");

	Entity musicEntity = m_CurrentScene->CreateEntity();
	SoundSource* musicSource = musicEntity.AddComponent<SoundSource>();
	musicSource->SetSound(musicSoundID);
	soundSources.push_back(musicSource);

	for (int i = 0; i < 2; i++)
	{
		Entity soundEntity = m_CurrentScene->CreateEntity();
		soundEntity.AddComponent<Transform>();
		SoundSource* source = soundEntity.AddComponent<SoundSource>();
		source->SetSound(bellSoundID);

		soundSources.push_back(source);
	}

	// Add scene to active scenes
	SceneSystem::UnloadAllScenes();
	SceneSystem::AddScene(m_CurrentScene);
}

void EditorApp::LoadProject()
{
	string projectPath = fmt::format("file://{}/project.json", m_ProjectPath.string());
	replace(projectPath.begin(), projectPath.end(), '\\', '/');

	m_ProjectInfo = ReadProject(projectPath);

	if(m_ProjectInfo.Name.empty())
	{
		// Failed to load, exit application
		Exit();
		return;
	}
	else
		// Successfully loaded
		spdlog::info("Loaded project '{}'", m_ProjectInfo.Name.c_str());

	for(const string& assembly : m_ProjectInfo.Assemblies)
	{
		if(assembly.empty())
			continue;
		ScriptEngine::LoadAssembly(assembly, true);
	}
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

	int soundIndex = 0;
	for (SoundSource* soundSource : soundSources)
	{
		string id = "SoundSource [" + to_string(soundSource->Entity.ID()) + "]##" + to_string(soundIndex++);
		ImGui::Begin(id.c_str());

		string resourcePath = Resource::GetPath(soundSource->GetSound());
		ImGui::Text("'%s'", resourcePath.c_str());

		// Play
		if (soundSource->IsPlaying())
		{
			if (ImGui::Button("||")) soundSource->Pause();
		}
		else if (ImGui::Button(">"))
			soundSource->Play();

		// Stop
		ImGui::SameLine();
		if (ImGui::Button("[]")) soundSource->Stop();

		// Seek bar
		ImGui::SameLine();
		float soundValue = soundSource->GetPlayTime();
		float soundMaxValue = soundSource->GetLength();
		if (ImGui::SliderFloat("##seek", &soundValue, 0, soundMaxValue, ""))
			soundSource->Seek(soundValue);

		ImGui::SameLine();

		unsigned int valueMinutes = (unsigned int)std::floor(soundValue > 60 ? std::floor(soundValue / 60.0f) : 0.0f);
		unsigned int valueSeconds = (unsigned int)std::floor(soundValue > 60 ? (soundValue - valueMinutes * 60.0f) : soundValue);
		unsigned int maxValueMinutes = (unsigned int)std::floor(soundMaxValue > 60 ? std::floor(soundMaxValue / 60.0f) : 0.0f);
		unsigned int maxValueSeconds = (unsigned int)std::floor(soundMaxValue > 60 ? (soundMaxValue - maxValueMinutes * 60.0f) : soundMaxValue);
		ImGui::Text("%02i:%02i / %02i:%02i", valueMinutes, valueSeconds, maxValueMinutes, maxValueSeconds);

		int volumePercent = (int)std::floor(soundSource->GetVolume() * 100.0f);
		if (ImGui::SliderInt("Volume", &volumePercent, 0, 100, "%i%"))
			soundSource->SetVolume(volumePercent / 100.0f);

		Transform* transform = soundSource->Entity.GetComponent<Transform>();
		if(transform)
			ImGui::DragFloat3("Position", &transform->Position[0], 0.01f);

		ImGui::SameLine();
		bool looping = soundSource->IsLooping();
		if(ImGui::Checkbox("Loop", &looping))
			soundSource->SetLooping(looping);

		ImGui::SameLine();
		bool spatialise = soundSource->GetSpatialization();
		if(ImGui::Checkbox("Spatial", &spatialise))
			soundSource->SetSpatialization(spatialise);

		ImGui::End();
	}

	ImGui::End();
}