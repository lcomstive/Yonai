#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

namespace InternalCalls
{
	void TransformSetPosition(unsigned int worldID, unsigned int entityID, glm::vec3* value)
	{
		World* world = World::GetWorld(worldID);
		if (!world)
			return;
		Transform* transform = world->GetEntity(entityID).GetComponent<Transform>();
		if (transform)
			transform->Position = *value;
	}

	void TransformGetPosition(unsigned int worldID, unsigned int entityID, glm::vec3* outValue)
	{
		World* world = World::GetWorld(worldID);
		if (!world)
			return;
		Transform* transform = world->GetEntity(entityID).GetComponent<Transform>();
		if (transform)
			*outValue = transform->Position;
	}

	void TransformSetRotation(unsigned int worldID, unsigned int entityID, glm::quat* value)
	{
		World* world = World::GetWorld(worldID);
		if (!world)
			return;
		Transform* transform = world->GetEntity(entityID).GetComponent<Transform>();
		if (transform)
			transform->Rotation = *value;
	}

	void TransformGetRotation(unsigned int worldID, unsigned int entityID, glm::quat* outValue)
	{
		World* world = World::GetWorld(worldID);
		if (!world)
			return;
		Transform* transform = world->GetEntity(entityID).GetComponent<Transform>();
		if (transform)
			*outValue = transform->Rotation;
	}

	void TransformSetScale(unsigned int worldID, unsigned int entityID, glm::vec3* value)
	{
		World* world = World::GetWorld(worldID);
		if (!world)
			return;
		Transform* transform = world->GetEntity(entityID).GetComponent<Transform>();
		if (transform)
			transform->Scale = *value;
	}

	void TransformGetScale(unsigned int worldID, unsigned int entityID, glm::vec3* outValue)
	{
		World* world = World::GetWorld(worldID);
		if (!world)
			return;
		Transform* transform = world->GetEntity(entityID).GetComponent<Transform>();
		if (transform)
			*outValue = transform->Scale;
	}

	void AddTransformInternalCalls()
	{
		mono_add_internal_call("AquaEngine.Transform::_aqua_internal_Transform_SetScale", (const void*)TransformSetScale);
		mono_add_internal_call("AquaEngine.Transform::_aqua_internal_Transform_GetScale", (const void*)TransformGetScale);
		mono_add_internal_call("AquaEngine.Transform::_aqua_internal_Transform_SetPosition", (const void*)TransformSetPosition);
		mono_add_internal_call("AquaEngine.Transform::_aqua_internal_Transform_GetPosition", (const void*)TransformGetPosition);
		mono_add_internal_call("AquaEngine.Transform::_aqua_internal_Transform_SetRotation", (const void*)TransformSetRotation);
		mono_add_internal_call("AquaEngine.Transform::_aqua_internal_Transform_GetRotation", (const void*)TransformGetRotation);
	}
}