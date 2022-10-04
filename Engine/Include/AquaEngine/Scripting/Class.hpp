#pragma once
#include <mono/jit/jit.h>
#include <memory>
#include <AquaEngine/Scripting/Method.hpp>

namespace AquaEngine::Scripting
{
	struct Class
	{
		MonoClass* Handle;
		MonoObject* Instance;

		Class(MonoClass* handle, MonoObject* instance);

		MonoClassField* GetField(const char* name);
		MonoProperty* GetProperty(const char* name);
		std::unique_ptr<Method> GetMethod(const char* name, int parameterCount = 0);

	};
}