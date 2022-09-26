#pragma once
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

namespace AquaEngine::Systems
{
	struct CameraControlSystem : public System
	{
		AquaAPI void Update() override;
	};
}