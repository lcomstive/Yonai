#include <string>
#include <spdlog/spdlog.h>
#include <glm/gtx/quaternion.hpp>
#include <AquaEngine/Components/Camera.hpp>

using namespace std;
using namespace glm;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;

Camera* Camera::s_MainCamera = nullptr;

const vec3 WorldUp = { 0, 1, 0 };

mat4 Camera::GetViewMatrix()
{
	Transform* transform = Entity.GetComponent<Transform>();
	quat rotation = transform->Rotation;
	vec3 up = vec3(0, 1, 0) * rotation;
	vec3 forward = vec3(0, 0, 1) * rotation;
	return lookAt(transform->Position, transform->Position + forward, up);
}

mat4 Camera::GetProjectionMatrix(int resolutionWidth, int resolutionHeight)
{
	float aspectRatio = resolutionWidth / (float)resolutionHeight;
	if (Orthographic)
	{
		float width  = OrthographicSize * aspectRatio;
		float height = OrthographicSize;

		return ortho(-width, width, -height, height, Near, Far);
	}
	else
		return perspective(radians(FieldOfView), aspectRatio, Near, Far);
}
mat4 Camera::GetProjectionMatrix(glm::ivec2 resolution) { return GetProjectionMatrix(resolution.x, resolution.y); }

void Camera::FillShader(Shader* shader, ivec2 resolution)
{
	shader->Set("camera.ViewMatrix", GetViewMatrix());
	shader->Set("camera.ProjectionMatrix", GetProjectionMatrix(resolution));
	shader->Set("camera.Position", Entity.GetComponent<Transform>()->GetGlobalPosition());
}

void Camera::SetMainCamera() { s_MainCamera = this; }
Camera* Camera::GetMainCamera() { return s_MainCamera; }

#pragma region Internal Calls
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_GET_SET(Camera, Far, float)
ADD_MANAGED_GET_SET(Camera, Near, float)
ADD_MANAGED_GET_SET(Camera, FieldOfView, float)
ADD_MANAGED_GET_SET(Camera, Orthographic, bool)
ADD_MANAGED_GET_SET(Camera, OrthographicSize, float)
#pragma endregion