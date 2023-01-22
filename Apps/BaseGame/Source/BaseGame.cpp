#include <filesystem>
#include <BaseGame.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Model.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Components/FPSCamera.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/CameraControlSystem.hpp>

#include <glm/gtc/quaternion.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

using namespace std;
using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;

World* Scene;
Camera* camera;
Transform* cameraTransform;

void BaseGame::Setup()
{
	WindowedApplication::Setup();

	Window::SetTitle("Base Game");
	Window::SetVSync(false);

	VFS::Mount("/Assets", "./Assets");

	Scene = new World();

	// 2D Sprite Test
	ResourceID textureID = Resource::Load<Texture>("Texture/Test", "/Assets/Textures/Test.png");
	ResourceID spriteShader = Resource::Load<Shader>("Shaders/Sprite", ShaderStageInfo
		{
			"/Assets/Shaders/Sprite.vert",
			"/Assets/Shaders/Sprite.frag"
		});
	const unsigned int spriteRows = 10;
	const unsigned int spriteColumns = 10;
	for (unsigned int x = 0; x < spriteRows; x++)
	{
		for (unsigned int y = 0; y < spriteColumns; y++)
		{
			Entity entity = Scene->CreateEntity();

			Transform* transform = entity.AddComponent<Transform>();
			transform->Position = { x - (spriteRows / 2.0f), y - (spriteColumns / 2.0f), 0 };
			transform->Scale = { 0.15f, 0.15f, 0.15f };

			SpriteRenderer* sprite = entity.AddComponent<SpriteRenderer>();
			sprite->Sprite = textureID;
			sprite->Shader = spriteShader;
		}
	}

	// 3D Model Load Test
	const int rows = 10;
	const int columns = 10;
	for(int x = 0; x < rows; x++)
	{
		for(int y = 0; y < rows; y++)
		{
			Entity modelEntity = Scene->CreateEntity();

			Transform* modelTransform = modelEntity.AddComponent<Transform>();
			modelTransform->Scale = { 0.15f, 0.15f, 0.15f };
			modelTransform->Position =
			{
				x - (rows / 2.0f) + 0.5f,
				y - (rows / 2.0f) + 0.5f,
				0
			};

			MeshRenderer* modelRenderer = modelEntity.AddComponent<MeshRenderer>();
			modelRenderer->Mesh = Mesh::Cube();
			modelRenderer->Material = Resource::Load<Material>("Materials/Model");
			Material* modelMaterial = Resource::Get<Material>(modelRenderer->Material);
			modelMaterial->Shader = Resource::Load<Shader>("Shaders/Lit", ShaderStageInfo
				{
					"/Assets/Shaders/Lit.vert",
					"/Assets/Shaders/Lit.frag"
				});
		}
	}

	// Add a camera
	Entity cameraEntity = Scene->CreateEntity();
	cameraTransform = cameraEntity.AddComponent<Transform>();
	cameraTransform->Position = { 0, 0, -10 };
	camera = cameraEntity.AddComponent<Camera>();
	camera->Orthographic = false;
	cameraEntity.AddComponent<FPSCamera>();

	Scene->GetSystemManager()->Add<CameraControlSystem>();

	// Add scene to active scenes
	SystemManager::Global()->Get<SceneSystem>()->AddScene(Scene);
}

void BaseGame::Cleanup()
{
	delete Scene;
}

bool IsQuitKeyPressed()
{
	/* Keyboard (Escape) */
	if (Input::IsKeyPressed(Key::Escape))
		return true;

	/* Gamepad (Start + Back) */
	if (Input::IsGamepadConnected(0) &&
		Input::IsButtonDown(0, GamepadButton::Start) &&
		Input::IsButtonDown(0, GamepadButton::Back))
		return true;
	return false;
}

const float ScrollSpeed = 30.0f;

void BaseGame::OnUpdate()
{
	float deltaTime = (float)Time::DeltaTime();

	Window::SetTitle("Base Game [" + to_string((int)Time::FPS()) + " FPS]");

	// Quit Key //
	if (IsQuitKeyPressed())
		Exit();

	// Toggle Fullscreen //
	if (Input::IsKeyPressed(Key::F11))
		Window::SetFullscreen(Window::GetFullscreen() == FullscreenMode::None ?
			FullscreenMode::Borderless : FullscreenMode::None);

	// Toggle Orthographic //
	if (Input::IsKeyPressed(Key::Space))
		camera->Orthographic = !camera->Orthographic;

	if (camera->Orthographic)
		camera->OrthographicSize -= ScrollSpeed * Input::GetScrollDelta() * deltaTime;

	GetRenderSystem()->GetPipeline()->SetResolution(Window::GetFramebufferResolution());
}

#include <imgui.h>
void BaseGame::OnDraw()
{
	// Blit output to default framebuffer (screen)
	GetRenderSystem()->GetPipeline()->GetOutput()->BlitTo(nullptr);

	ImGui::Begin("FPS");
	ImGui::Text("FPS: %.2f", Time::FPS());
	ImGui::End();
}
