#include <filesystem>
#include <BaseGame.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Model.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Systems/SceneSystem.hpp>

#include <glm/gtc/quaternion.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

using namespace std;
using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;

Camera* camera;
Transform* cameraTransform;
World* Scene;

BaseGame::BaseGame(int argc, char** argv) : WindowedApplication() {}

void BaseGame::Setup()
{
	WindowedApplication::Setup();

	Window::SetTitle("Base Game");
	Window::SetVSync(false);

	Scene = new World();

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
			Entity entity = Scene->CreateEntity();

			Transform* transform = entity.AddComponent<Transform>();
			transform->Position = { x - (spriteRows / 2.0f), y - (spriteColumns / 2.0f), 0 };
			transform->Scale = { 0.15f, 0.15f, 0.15f };
			transform->Rotation = quat(vec3(3.14159f, 0, 0));

			SpriteRenderer* sprite = entity.AddComponent<SpriteRenderer>();
			sprite->Sprite = textureID;
			sprite->Shader = spriteShader;
		}
	}

	// 3D Model Load Test
	const int rows = 20;
	const int columns = 20;
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

	// Add scene to active scenes
	SystemManager::Get<SceneSystem>()->AddScene(Scene);
}

BaseGame::~BaseGame()
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

const float ControllerDeadzone = 0.1f;
glm::vec2 GetPlayerMovement()
{
	glm::vec2 movement = { 0, 0 };

	if (Input::IsKeyDown(Key::W) || Input::IsKeyDown(Key::ArrowUp)) 	movement.y += 1;
	if (Input::IsKeyDown(Key::A) || Input::IsKeyDown(Key::ArrowLeft)) 	movement.x -= 1;
	if (Input::IsKeyDown(Key::S) || Input::IsKeyDown(Key::ArrowDown)) 	movement.y -= 1;
	if (Input::IsKeyDown(Key::D) || Input::IsKeyDown(Key::ArrowRight)) 	movement.x += 1;

	// Check if a gamepad is connected
	if (Input::IsGamepadConnected(0))
	{
		// Left stick X axis
		float gamepadAxis = Input::GetAxis(0, GamepadAxis::AxisLeftX);
		if (abs(gamepadAxis) > ControllerDeadzone) movement.x += gamepadAxis;

		// Left stick Y axis
		gamepadAxis = Input::GetAxis(0, GamepadAxis::AxisLeftY);
		if (abs(gamepadAxis) > ControllerDeadzone) movement.y -= gamepadAxis;
	}

	return movement;
}

void BaseGame::OnUpdate()
{
	Window::SetTitle("Base Game [" + to_string((int)Time::FPS()) + " FPS]");

	// Quit Key //
	if (IsQuitKeyPressed())
		Exit();

	// Toggle Fullscreen //
	if (Input::IsKeyPressed(Key::F11))
		Window::SetFullscreen(Window::GetFullscreen() == FullscreenMode::None ?
			FullscreenMode::Borderless : FullscreenMode::None);

	if (camera->Orthographic)
		camera->OrthographicSize -= Input::GetScrollDelta();
	else
		cameraTransform->Position.z += Input::GetScrollDelta();

	// Player Movement //
	glm::vec2 movement = GetPlayerMovement() * (float)Time::DeltaTime();
	cameraTransform->Position.x += movement.x;
	cameraTransform->Position.y -= movement.y;
}
