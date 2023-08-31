#pragma once
#include <mono/jit/jit.h>

// Forward declaration
struct _MonoType;
struct _MonoObject;

namespace Yonai::Scripting
{
	struct ManagedData
	{
		/// <summary>
		/// When true, try to call message functions such as Update & Draw on Instance every frame
		/// </summary>
		bool ShouldSendMessages = false;

		/// <summary>
		/// Garbage collection handle, used to prevent premature destruction of component objects
		/// </summary>
		unsigned int GCHandle = 0;

		/// <summary>
		/// The managed type of Instance
		/// </summary>
		MonoType* Type = nullptr;

		MonoObject* GetInstance() { return GCHandle == 0 ? nullptr : mono_gchandle_get_target(GCHandle); }

		bool IsValid() { return GCHandle != 0; }
	};
}