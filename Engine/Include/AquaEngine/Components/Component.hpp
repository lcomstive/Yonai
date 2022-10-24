#pragma once
#include <AquaEngine/World.hpp>

struct _MonoObject;

namespace AquaEngine::Components
{
	struct Component
	{
		/// <summary>
		/// Entity this component is attached to
		/// </summary>
		AquaEngine::Entity Entity;

		/// <summary>
		/// A reference to the managed instance of this component.
		/// Handled by scripting engine.
		/// </summary>
		_MonoObject* ManagedInstance = nullptr;
	};
}
