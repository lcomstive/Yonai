#pragma once
#include <string>
#include <Yonai/Components/Component.hpp>

namespace Yonai::Components
{
	struct DebugName : public Component
	{
		std::string Name = "";
	};
}
