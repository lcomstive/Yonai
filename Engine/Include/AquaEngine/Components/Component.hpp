#pragma once
#include <AquaEngine/World.hpp>
#include <AquaEngine/Scripting/ManagedData.hpp>

namespace AquaEngine::Components
{
	struct Component
	{
		bool Enabled = false;

		/// <summary>
		/// Entity this component is attached to
		/// </summary>
		AquaEngine::Entity Entity;

		AquaEngine::Scripting::ManagedData ManagedData;

		// Have virtual destructor for components to override
		AquaAPI virtual ~Component() {}
	};
}
