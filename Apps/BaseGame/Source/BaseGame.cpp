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
	Window::SetVSync(true);

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
	cameraTransform->Rotation = quat(vec3(0, 0, 0));
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

const float ScrollSpeed = 30.0f;
const float ControllerDeadzone = 0.1f;
glm::vec3 GetPlayerMovement(float deltaTime)
{
	glm::vec3 movement = { 0, 0, 0 };

	if (Input::IsKeyDown(Key::W) || Input::IsKeyDown(Key::ArrowUp)) 	movement.z += deltaTime;
	if (Input::IsKeyDown(Key::S) || Input::IsKeyDown(Key::ArrowDown)) 	movement.z -= deltaTime;
	if (Input::IsKeyDown(Key::A) || Input::IsKeyDown(Key::ArrowLeft)) 	movement.x += deltaTime;
	if (Input::IsKeyDown(Key::D) || Input::IsKeyDown(Key::ArrowRight)) 	movement.x -= deltaTime;
	if (Input::IsKeyDown(Key::E)) 										movement.y += deltaTime;
	if (Input::IsKeyDown(Key::Q)) 										movement.y -= deltaTime;

	float scrollDelta = Input::GetScrollDelta();
	if (!camera->Orthographic && abs(scrollDelta) > 0.1f)
		movement.z += ScrollSpeed * scrollDelta * deltaTime;

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

	float speedMultiplier = Input::IsKeyDown(Key::LeftShift) ? 2.0f : 1.0f;

	if (camera->Orthographic)
		camera->OrthographicSize -= ScrollSpeed * Input::GetScrollDelta() * deltaTime * speedMultiplier;

	// Player Movement //
	cameraTransform->Position += inverse(cameraTransform->Rotation) * GetPlayerMovement(deltaTime) * speedMultiplier;

	Input::ShowMouse(!Input::IsMouseDown(MouseButton::Right));
	if (Input::IsMouseDown(MouseButton::Right))
	{
		static float xRot = 0, yRot = 0;

		vec2 mouseDelta = Input::GetMouseDelta() * deltaTime;
		xRot -= mouseDelta.y;
		yRot += mouseDelta.x;

		quat pitch	= angleAxis(xRot, vec3(1, 0, 0));
		quat yaw	= angleAxis(yRot, vec3(0, 1, 0));

		cameraTransform->Rotation = normalize(pitch * yaw);
	}
}
