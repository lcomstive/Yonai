#pragma once
#include <Yonai/World.hpp>
#include <Yonai/Scripting/ManagedData.hpp>

namespace Yonai::Components
{
	struct Component
	{
		/// <summary>
		/// Entity this component is attached to
		/// </summary>
		Yonai::Entity Entity;

		Yonai::Scripting::ManagedData ManagedData;

		// Have virtual destructor for components to override
		YonaiAPI virtual ~Component() {}
	};
}
