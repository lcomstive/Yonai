#pragma once
#include <string>
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine::Components
{
	struct DebugName : public Component
	{
		std::string Name = "";
	};
}
