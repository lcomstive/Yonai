#include <imgui.h>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Input.hpp>
#include <glm/gtc/quaternion.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Components/FPSCamera.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/CameraControlSystem.hpp>

using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Components;

const float ControllerDeadzone = 0.1f;

vec3 GetPlayerMovement();
quat GetCameraRotation(float deltaTime);

void CameraControlSystem::Update()
{
	float deltaTime = (float)Time::DeltaTime();
	vec3 movement = GetPlayerMovement() * deltaTime;
	quat rotation = GetCameraRotation(deltaTime);

	auto cameras = GetWorld()->GetComponents<Transform, FPSCamera>();
	for(auto cameraPair : cameras)
	{
		vec3 totalMovement = movement * cameraPair.second->Speed;
		if(Input::IsKeyDown(Key::LeftShift))
			totalMovement *= cameraPair.second->SprintMultiplier;
		cameraPair.first->Position += inverse(cameraPair.first->Rotation) * totalMovement;
		cameraPair.first->Rotation = rotation;
	}
}

vec3 GetPlayerMovement()
{
	vec3 movement = { 0, 0, 0 };

	if (Input::IsKeyDown(Key::W) || Input::IsKeyDown(Key::ArrowUp)) 	movement.z += 1;
	if (Input::IsKeyDown(Key::S) || Input::IsKeyDown(Key::ArrowDown)) 	movement.z -= 1;
	if (Input::IsKeyDown(Key::A) || Input::IsKeyDown(Key::ArrowLeft)) 	movement.x += 1;
	if (Input::IsKeyDown(Key::D) || Input::IsKeyDown(Key::ArrowRight)) 	movement.x -= 1;
	if (Input::IsKeyDown(Key::E)) 										movement.y += 1;
	if (Input::IsKeyDown(Key::Q)) 										movement.y -= 1;

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

quat GetCameraRotation(float deltaTime)
{
	static float xRot = 0, yRot = 0;

	Input::ShowMouse(!Input::IsMouseDown(MouseButton::Right));
	if(Input::IsMouseDown(MouseButton::Right))
	{
		vec2 mouseDelta = Input::GetMouseDelta() * deltaTime;
		xRot -= mouseDelta.y;
		yRot += mouseDelta.x;
	}

	quat pitch	= angleAxis(xRot, vec3(1, 0, 0));
	quat yaw	= angleAxis(yRot, vec3(0, 1, 0));

	return normalize(pitch * yaw);
}