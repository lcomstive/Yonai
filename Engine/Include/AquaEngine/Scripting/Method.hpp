#pragma once
#include <memory>
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Scripting
{
	struct Method
	{
		MonoMethod* Handle;

		AquaAPI Method(MonoMethod* handle);

		AquaAPI void Invoke(MonoObject* instance = nullptr, void** params = nullptr);

		template<typename T>
		void Invoke(T* param) { Invoke(nullptr, (void**)&param); }

		template<typename T>
		void Invoke(T param) { Invoke<T>(&param); }

		template<typename T>
		void Invoke(MonoObject* instance, T* param) { Invoke(instance, (void**)&param); }

		template<typename T>
		void Invoke(MonoObject* instance, T param) { Invoke<T>(instance, &param); }
	};
}