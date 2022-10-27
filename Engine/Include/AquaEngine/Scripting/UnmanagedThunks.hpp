#pragma once
#include <mono/jit/jit.h>

namespace AquaEngine::Scripting
{
	typedef void (*ComponentMethodStartFn)(MonoObject*, MonoException**);
	typedef void (*ComponentMethodUpdateFn)(MonoObject*, MonoException**);
	typedef void (*ComponentMethodDestroyedFn)(MonoObject*, MonoException**);
	typedef void (*ComponentMethodEnabledFn)(MonoObject*, bool, MonoException**);
	typedef void (*ComponentMethodInitialiseFn)(MonoObject*, unsigned int, unsigned int, MonoException**);

	typedef void (*SystemMethodDrawFn)(MonoObject*, MonoException**);
	typedef void (*SystemMethodStartFn)(MonoObject*, MonoException**);
	typedef void (*SystemMethodUpdateFn)(MonoObject*, MonoException**);
	typedef void (*SystemMethodDestroyedFn)(MonoObject*, MonoException**);
	typedef void (*SystemMethodEnabledFn)(MonoObject*, bool, MonoException**);
	typedef void (*SystemMethodInitialiseFn)(MonoObject*, unsigned int, unsigned int, MonoException**);
}