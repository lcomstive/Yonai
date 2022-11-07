#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

Transform* GetTransform(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	if (!world) return nullptr;
	return world->GetEntity(entityID).GetComponent<Transform>();
}

void SetPosition(unsigned int worldID, unsigned int entityID, glm::vec3* value)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		transform->Position = *value;
}

void GetPosition(unsigned int worldID, unsigned int entityID, glm::vec3* outValue)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Position;
}

void SetRotation(unsigned int worldID, unsigned int entityID, glm::quat* value)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		transform->Rotation = *value;
}

void GetRotation(unsigned int worldID, unsigned int entityID, glm::quat* outValue)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Rotation;
}

void SetScale(unsigned int worldID, unsigned int entityID, glm::vec3* value)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		transform->Scale = *value;
}

void GetScale(unsigned int worldID, unsigned int entityID, glm::vec3* outValue)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Scale;
}

void GetUp(unsigned int worldID, unsigned int entityID, glm::vec3* outValue)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Up();
}

void GetForward(unsigned int worldID, unsigned int entityID, glm::vec3* outValue)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Forward();
}

void GetRight(unsigned int worldID, unsigned int entityID, glm::vec3* outValue)
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Right();
}

#define ADD_TRANSFORM_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.Transform::_aqua_internal_Transform_"#name, (const void*)name);

void AquaEngine::Scripting::Assembly::AddTransformInternalCalls()
{
	ADD_TRANSFORM_INTERNAL_CALL(SetScale);
	ADD_TRANSFORM_INTERNAL_CALL(GetScale);
	ADD_TRANSFORM_INTERNAL_CALL(SetPosition);
	ADD_TRANSFORM_INTERNAL_CALL(GetPosition);
	ADD_TRANSFORM_INTERNAL_CALL(SetRotation);
	ADD_TRANSFORM_INTERNAL_CALL(GetRotation);
	
	ADD_TRANSFORM_INTERNAL_CALL(GetUp);
	ADD_TRANSFORM_INTERNAL_CALL(GetForward);
	ADD_TRANSFORM_INTERNAL_CALL(GetRight);
}