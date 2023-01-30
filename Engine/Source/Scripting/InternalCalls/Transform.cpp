#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

Transform* GetTransform(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	if (!world) return nullptr;
	return world->GetEntity(entityID).GetComponent<Transform>();
}

ADD_MANAGED_METHOD(Transform, SetPosition, void, (unsigned int worldID, unsigned int entityID, glm::vec3* value))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		transform->Position = *value;
}

ADD_MANAGED_METHOD(Transform, GetPosition, void, (unsigned int worldID, unsigned int entityID, glm::vec3* outValue))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Position;
}

ADD_MANAGED_METHOD(Transform, SetRotation, void, (unsigned int worldID, unsigned int entityID, glm::quat* value))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		transform->Rotation = *value;
}

ADD_MANAGED_METHOD(Transform, GetRotation, void, (unsigned int worldID, unsigned int entityID, glm::quat* outValue))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Rotation;
}

ADD_MANAGED_METHOD(Transform, SetScale, void, (unsigned int worldID, unsigned int entityID, glm::vec3* value))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		transform->Scale = *value;
}

ADD_MANAGED_METHOD(Transform, GetScale, void, (unsigned int worldID, unsigned int entityID, glm::vec3* outValue))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Scale;
}

ADD_MANAGED_METHOD(Transform, GetUp, void, (unsigned int worldID, unsigned int entityID, glm::vec3* outValue))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Up();
}

ADD_MANAGED_METHOD(Transform, GetForward, void, (unsigned int worldID, unsigned int entityID, glm::vec3* outValue))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Forward();
}

ADD_MANAGED_METHOD(Transform, GetRight, void, (unsigned int worldID, unsigned int entityID, glm::vec3* outValue))
{
	Transform* transform = GetTransform(worldID, entityID);
	if (transform)
		*outValue = transform->Right();
}
