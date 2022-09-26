#pragma once
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Components/FPSCamera.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

namespace AquaEngine::Systems
{
	struct CameraControlSystem : public System
	{
		AquaAPI void Update(Components::FPSCamera* fpsCamera);

		AquaAPI void Update() override;
	};
}