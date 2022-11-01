#pragma once
#include <AquaEngine/World.hpp>

struct _MonoType;
struct _MonoObject;

namespace AquaEngine::Components
{
	struct ManagedComponentData
	{
		/// <summary>
		/// A reference to the managed instance of this component.
		/// Handled by scripting engine.
		/// </summary>
		_MonoObject* Instance = nullptr;

		/// <summary>
		/// When true, calls Component::Update() on ManagedInstance
		/// </summary>
		bool ShouldCallUpdate = false;

		/// <summary>
		/// Garbage collection handle, used to prevent premature destruction of component objects
		/// </summary>
		unsigned int GCHandle;

		/// <summary>
		/// The managed type of ManagedInstance
		/// </summary>
		_MonoType* Type = nullptr;
	};

	struct Component
	{
		/// <summary>
		/// Entity this component is attached to
		/// </summary>
		AquaEngine::Entity Entity;

		ManagedComponentData ManagedData;
	};
}
