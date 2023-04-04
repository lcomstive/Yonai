#pragma once
#include <mono/jit/jit.h>

namespace AquaEngine::Scripting
{
	typedef void (*EmptyMethodFn)(MonoObject*, MonoException**);

	typedef void (*ComponentMethodEnabledFn)(MonoObject*, bool, MonoException**);
	typedef void (*ComponentMethodInitialiseFn)(MonoObject*, uint64_t, uint64_t, MonoException**);

	typedef void (*SystemMethodEnabledFn)(MonoObject*, bool, MonoException**);
	typedef void (*SystemMethodInitialiseFn)(MonoObject*, uint64_t, MonoException**);
}