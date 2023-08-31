#pragma once
#include <memory>
#include <mono/jit/jit.h>
#include <Yonai/API.hpp>
#include <Yonai/Scripting/Method.hpp>

namespace Yonai::Scripting
{
	struct Class
	{
		MonoClass* Handle;
		MonoObject* Instance;

		YonaiAPI Class(MonoClass* handle, MonoObject* instance);

		YonaiAPI MonoClassField* GetField(const char* name);
		YonaiAPI MonoProperty* GetProperty(const char* name);
		YonaiAPI Method GetMethod(const char* name, int parameterCount = 0);
		YonaiAPI MonoMethod* GetMonoMethod(const char* name, int parameterCount = 0);

		YonaiAPI void Invoke(MonoMethod* method, void** params = nullptr);
		YonaiAPI void Invoke(const char* methodName, void** params = nullptr);

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