#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

ADD_MANAGED_METHOD(Camera, SetMainCamera, void, (void* handle))
{ ((Camera*)handle)->SetMainCamera(); }

ADD_MANAGED_METHOD(Camera, GetMainCamera, void, (unsigned int* worldID, unsigned int* entityID))
{
	Camera* mainCamera = Camera::GetMainCamera();
	*worldID = mainCamera ? mainCamera->Entity.GetWorld()->ID() : InvalidEntityID;
	*entityID = mainCamera ? mainCamera->Entity.ID() : InvalidEntityID;
}
