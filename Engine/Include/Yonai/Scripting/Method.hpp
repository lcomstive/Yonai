#pragma once
#include <memory>
#include <mono/jit/jit.h>
#include <Yonai/API.hpp>

namespace Yonai::Scripting
{
	struct Method
	{
		MonoMethod* Handle;

		YonaiAPI Method(MonoMethod* handle);

		YonaiAPI MonoObject* Invoke(MonoObject* instance = nullptr, void** params = nullptr);

		template<typename T>
		MonoObject* Invoke(T* param) { return Invoke(nullptr, (void**)&param); }

		template<typename T>
		MonoObject* Invoke(T param) { return Invoke<T>(&param); }

		template<typename T>
		MonoObject* Invoke(MonoObject* instance, T* param) { return Invoke(instance, (void**)&param); }

		template<typename T>
		MonoObject* Invoke(MonoObject* instance, T param) { return Invoke<T>(instance, &param); }
	};
}