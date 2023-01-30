#pragma once
#include <memory>
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Scripting
{
	struct Class
	{
		MonoClass* Handle;
		MonoObject* Instance;

		AquaAPI Class(MonoClass* handle, MonoObject* instance);

		AquaAPI MonoClassField* GetField(const char* name);
		AquaAPI MonoProperty* GetProperty(const char* name);
		AquaAPI MonoMethod* GetMethod(const char* name, int parameterCount = 0);

		AquaAPI void Invoke(MonoMethod* method, void** params = nullptr);
		AquaAPI void Invoke(const char* methodName, void** params = nullptr);

		template<typename T>
		void Invoke(const char* name, T* param) { Invoke(GetMethod(name, 1), &param); }

		template<typename T>
		void Invoke(MonoMethod* method, T* param) { Invoke(method, &param); }

		template<typename T>
		void Invoke(const char* name, T param) { Invoke<T>(name, &param); }

		template<typename T>
		void Invoke(MonoMethod* method, T param) { Invoke<T>(method, &param); }
	};
}