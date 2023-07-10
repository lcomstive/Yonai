#include <string>
#include <spdlog/spdlog.h>
#include <glm/gtx/quaternion.hpp>
#include <Yonai/Components/Camera.hpp>

using namespace std;
using namespace glm;
using namespace Yonai;
using namespace Yonai::Graphics;
using namespace Yonai::Components;

const vec3 WorldUp = { 0, 1, 0 };

mat4 Camera::GetViewMatrix()
{
	Transform* transform = Entity.GetComponent<Transform>();
	if (!transform)
		return mat4(1.0f);

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
	if (!shader)
		return;
	shader->Set("camera.ViewMatrix", GetViewMatrix());
	shader->Set("camera.ProjectionMatrix", GetProjectionMatrix(resolution));

	Transform* transform = Entity.GetComponent<Transform>();
	shader->Set("camera.Position", transform ? transform->GetGlobalPosition() : vec3(0, 0, 0));
}

#pragma region Internal Calls
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_GET_SET(Camera, Far, float)
ADD_MANAGED_GET_SET(Camera, Near, float)
ADD_MANAGED_GET(Camera, RenderTarget, void*)
ADD_MANAGED_GET_SET(Camera, FieldOfView, float)
ADD_MANAGED_GET_SET(Camera, Orthographic, bool)
ADD_MANAGED_GET_SET(Camera, OrthographicSize, float)

ADD_MANAGED_METHOD(Camera, SetRenderTarget, void, (void* handle, void* renderTarget))
{
	((Camera*)handle)->RenderTarget = (RenderTexture*)renderTarget;
	spdlog::trace("Camera render target set to {}", (renderTarget ? to_string(((RenderTexture*)renderTarget)->GetID()) : "(null)"));
}
#pragma endregion