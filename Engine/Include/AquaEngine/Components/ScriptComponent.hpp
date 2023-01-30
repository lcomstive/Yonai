#pragma once
#include <memory>
#include <AquaEngine/Scripting/Class.hpp>
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine::Components
{
	struct ScriptComponent : public Component
	{
		MonoType* Type = nullptr;
	};
}