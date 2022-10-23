#pragma once
#include <memory>
#include <mono/metadata/assembly.h>
#include <AquaEngine/Scripting/Class.hpp>

namespace AquaEngine::Scripting
{
	class ScriptSystem;

	struct Assembly
	{
		ScriptSystem* Owner;
		MonoAssembly* Handle;
		MonoImage* Image;

		Assembly(ScriptSystem* owner, MonoAssembly* handle);

		std::unique_ptr<Class> InstantiateClass(const char* namespaceName, const char* className);
	
	private:
		void AddInternalCalls();
	};
}