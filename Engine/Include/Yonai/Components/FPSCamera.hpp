#pragma once
#include <Yonai/Components/Component.hpp>

namespace Yonai::Components
{
	struct FPSCamera : public Component
	{
		float Speed = 1.0f;
		float SprintMultiplier = 2.5f;
	};
}