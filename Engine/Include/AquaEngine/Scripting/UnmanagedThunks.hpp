#pragma once
#include <mono/jit/jit.h>

namespace AquaEngine::Scripting
{
	typedef void (*EmptyMethodFn)(MonoObject*, MonoException**);

	typedef void (*ComponentMethodEnabledFn)(MonoObject*, bool, MonoException**);
	typedef void (*ComponentMethodInitialiseFn)(MonoObject*, unsigned int, unsigned int, MonoException**);

	typedef void (*SystemMethodEnabledFn)(MonoObject*, bool, MonoException**);
	typedef void (*SystemMethodInitialiseFn)(MonoObject*, unsigned int, MonoException**);
}