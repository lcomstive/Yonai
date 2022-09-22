#include <AquaEngine/World.hpp>
#include <glm/gtx/quaternion.hpp>
#include <AquaEngine/Components/Transform.hpp>

using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;

vec3 Transform::GetGlobalPosition()
{
	vec3 position = Position;

	Transform* parentTransform = Parent;
	while (parentTransform)
	{
		position += parentTransform->Position;
		parentTransform = parentTransform->Parent;
	}

	return position;
}

quat Transform::GetGlobalRotation()
{
	quat rotation = Rotation;

	Transform* parentTransform = Parent;
	while (parentTransform)
	{
		rotation = parentTransform->Rotation * rotation;
		parentTransform = parentTransform->Parent;
	}

	return rotation;
}

vec3 Transform::GetGlobalScale()
{
	vec3 scale = Scale;

	Transform* parentTransform = Parent;
	while (parentTransform)
	{
		scale *= parentTransform->Scale;
		parentTransform = parentTransform->Parent;
	}

	return scale;
}
vec3 Transform::Up() { return vec3(0, 1, 0) * Rotation; }
vec3 Transform::Right() { return vec3(1, 0, 0) * Rotation; }
vec3 Transform::Forward() { return vec3(0, 0, 1) * Rotation; }

mat4 Transform::GetModelMatrix()
{
	// Generate model matrix
	quat globalRot = GetGlobalRotation();
	mat4 translationMatrix = translate(mat4(1.0f), GetGlobalPosition());
	mat4 scaleMatrix = scale(mat4(1.0f), GetGlobalScale());
	mat4 rotationMatrix = toMat4(globalRot);
	
	return translationMatrix * scaleMatrix * rotationMatrix;
}
