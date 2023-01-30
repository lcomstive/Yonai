#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

Camera* GetCamera(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	if (!world)
		return nullptr;
	return world->GetEntity(entityID).GetComponent<Camera>();
}

ADD_MANAGED_METHOD(Camera, GetFOV, float, (unsigned int worldID, unsigned int entityID))
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->FieldOfView : -1;
}

ADD_MANAGED_METHOD(Camera, SetFOV, void, (unsigned int worldID, unsigned int entityID, float value))
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->FieldOfView = value;
}

ADD_MANAGED_METHOD(Camera, GetNear, float, (unsigned int worldID, unsigned int entityID))
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->Near : -1;
}

ADD_MANAGED_METHOD(Camera, SetNear, void, (unsigned int worldID, unsigned int entityID, float value))
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->Near = value;
}

ADD_MANAGED_METHOD(Camera, GetFar, float, (unsigned int worldID, unsigned int entityID))
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->Far : -1;
}

ADD_MANAGED_METHOD(Camera, SetFar, void, (unsigned int worldID, unsigned int entityID, float value))
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->Far = value;
}

ADD_MANAGED_METHOD(Camera, GetOrtho, float, (unsigned int worldID, unsigned int entityID))
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->Orthographic : false;
}

ADD_MANAGED_METHOD(Camera, SetOrtho, void, (unsigned int worldID, unsigned int entityID, bool value))
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->Orthographic = value;
}

ADD_MANAGED_METHOD(Camera, GetOrthoSize, float, (unsigned int worldID, unsigned int entityID))
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->OrthographicSize : 0;
}

ADD_MANAGED_METHOD(Camera, SetOrthoSize, void, (unsigned int worldID, unsigned int entityID, float value))
{
	Camera* camera = GetCamera(worldID, entityID);
	if (camera)
		camera->OrthographicSize = value;
}

ADD_MANAGED_METHOD(Camera, SetMainCamera, void, (unsigned int worldID, unsigned int entityID))
{
	Camera* camera = GetCamera(worldID, entityID);
	if (camera)
		camera->SetMainCamera();
}

ADD_MANAGED_METHOD(Camera, GetMainCamera, void, (unsigned int* worldID, unsigned int* entityID))
{
	Camera* mainCamera = Camera::GetMainCamera();
	*worldID = mainCamera ? mainCamera->Entity.GetWorld()->ID() : InvalidEntityID;
	*entityID = mainCamera ? mainCamera->Entity.ID() : InvalidEntityID;
}
