#pragma once
#include <string>
#include <memory>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace AquaEditor
{
	struct Class
	{
		MonoClass* Handle;
		MonoObject* Instance;

		Class(MonoClass* handle, MonoObject* instance);

		MonoClassField* GetField(const char* name);
		MonoProperty* GetProperty(const char* name);
		MonoMethod* GetMethod(const char* name, int parameterCount = 0);
	};

	struct Assembly
	{
		MonoAssembly* Handle;

		Assembly(MonoAssembly* handle);

		std::unique_ptr<Class> InstantiateClass(const char* namespaceName, const char* className);
	};

	struct ScriptEngine
	{
		static MonoDomain* AppDomain;
		static MonoDomain* RootDomain;

		static void InitMono();
		static std::unique_ptr<Assembly> LoadAssembly(const std::string& path);

		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
	};
}