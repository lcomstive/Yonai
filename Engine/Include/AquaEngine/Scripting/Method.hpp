#pragma once
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Scripting
{
	struct Method
	{
		MonoMethod* Handle;
		MonoObject* ClassInstance;

		AquaAPI Method(MonoObject* classInstance, MonoMethod* handle) : ClassInstance(classInstance), Handle(handle) { }

		AquaAPI void Invoke(void** params = nullptr);

		template<typename T>
		void Invoke(T* param) { Invoke(&param); }

		template<typename T>
		void Invoke(T param)
		{
			T* ptr = &param;
			Invoke<T>(ptr);
		}

		AquaAPI bool IsValid();
	};
}