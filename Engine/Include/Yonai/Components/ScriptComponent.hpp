#pragma once
#include <memory>
#include <Yonai/Scripting/Class.hpp>
#include <Yonai/Components/Component.hpp>

namespace Yonai::Components
{
	struct ScriptComponent : public Component
	{
		MonoType* Type = nullptr;
	};
}