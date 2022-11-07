#pragma once
#include <AquaEngine/World.hpp>
#include <AquaEngine/Scripting/ManagedData.hpp>

namespace AquaEngine::Components
{
	struct Component
	{
		/// <summary>
		/// Entity this component is attached to
		/// </summary>
		AquaEngine::Entity Entity;

		AquaEngine::Scripting::ManagedData ManagedData;
	};
}
