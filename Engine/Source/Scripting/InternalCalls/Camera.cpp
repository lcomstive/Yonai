#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
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

float GetFOV(unsigned int worldID, unsigned int entityID)
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->FieldOfView : -1;
}

void SetFOV(unsigned int worldID, unsigned int entityID, float value)
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->FieldOfView = value;
}

float GetNear(unsigned int worldID, unsigned int entityID)
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->Near : -1;
}

void SetNear(unsigned int worldID, unsigned int entityID, float value)
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->Near = value;
}

float GetFar(unsigned int worldID, unsigned int entityID)
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->Far : -1;
}

void SetFar(unsigned int worldID, unsigned int entityID, float value)
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->Far = value;
}

bool GetOrtho(unsigned int worldID, unsigned int entityID)
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->Orthographic : false;
}

void SetOrtho(unsigned int worldID, unsigned int entityID, bool value)
{
	Camera* camera = GetCamera(worldID, entityID);
	if(camera)
		camera->Orthographic = value;
}

float GetOrthoSize(unsigned int worldID, unsigned int entityID)
{
	Camera* camera = GetCamera(worldID, entityID);
	return camera ? camera->OrthographicSize : 0;
}

void SetOrthoSize(unsigned int worldID, unsigned int entityID, float value)
{
	Camera* camera = GetCamera(worldID, entityID);
	if (camera)
		camera->OrthographicSize = value;
}

void SetMainCamera(unsigned int worldID, unsigned int entityID)
{
	Camera* camera = GetCamera(worldID, entityID);
	if (camera)
		camera->SetMainCamera();
}

void GetMainCamera(unsigned int* worldID, unsigned int* entityID)
{
	Camera* mainCamera = Camera::GetMainCamera();
	*worldID = mainCamera ? mainCamera->Entity.GetWorld()->ID() : InvalidEntityID;
	*entityID = mainCamera ? mainCamera->Entity.ID() : InvalidEntityID;
}

/*
#define ADD_CAMERA_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.Camera::_aqua_internal_Camera_"#name, (const void*)name);

namespace AquaEngine::Scripting::Internal
{
	void AddCameraInternalCalls()
	{
		ADD_CAMERA_INTERNAL_CALL(GetFOV)
		ADD_CAMERA_INTERNAL_CALL(SetFOV)
		ADD_CAMERA_INTERNAL_CALL(GetNear)
		ADD_CAMERA_INTERNAL_CALL(SetNear)
		ADD_CAMERA_INTERNAL_CALL(GetFar)
		ADD_CAMERA_INTERNAL_CALL(SetFar)
		ADD_CAMERA_INTERNAL_CALL(GetOrtho)
		ADD_CAMERA_INTERNAL_CALL(SetOrtho)
		ADD_CAMERA_INTERNAL_CALL(GetOrthoSize)
		ADD_CAMERA_INTERNAL_CALL(SetOrthoSize)
		ADD_CAMERA_INTERNAL_CALL(GetMainCamera)
		ADD_CAMERA_INTERNAL_CALL(SetMainCamera)
	}
}
*/