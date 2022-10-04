#pragma once
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Scripting
{
	struct Method
	{
		MonoClass* Instance;
		MonoMethod* Handle;

		AquaAPI Method(MonoClass* instance, MonoMethod* handle) : Instance(instance), Handle(handle) { }

		AquaAPI void Invoke(void** params = nullptr)
		{
			MonoObject* exception = nullptr;
			mono_runtime_invoke(Handle, Instance, nullptr, &exception);
		}

		template<typename T>
		void Invoke(T* param) { Invoke(&param); }

		template<typename T>
		void Invoke(T param)
		{
			T* ptr = &param;
			Invoke<T>(ptr);
		}
	};
}