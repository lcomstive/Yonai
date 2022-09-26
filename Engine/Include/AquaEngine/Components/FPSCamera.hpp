#pragma once
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine::Components
{
	struct FPSCamera : public Component
	{
		float Speed = 1.0f;
		float SprintMultiplier = 2.5f;
	};
}