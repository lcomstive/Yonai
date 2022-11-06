#pragma once

// Forward declaration
struct _MonoType;
struct _MonoObject;

namespace AquaEngine::Scripting
{
	struct ManagedData
	{
		/// <summary>
		/// A reference to the managed instance of this component.
		/// Handled by scripting engine.
		/// </summary>
		_MonoObject* Instance = nullptr;

		/// <summary>
		/// When true, try to call message functions such as Update & Draw on Instance every frame
		/// </summary>
		bool ShouldSendMessages = false;

		/// <summary>
		/// Garbage collection handle, used to prevent premature destruction of component objects
		/// </summary>
		unsigned int GCHandle;

		/// <summary>
		/// The managed type of Instance
		/// </summary>
		_MonoType* Type = nullptr;
	};
}