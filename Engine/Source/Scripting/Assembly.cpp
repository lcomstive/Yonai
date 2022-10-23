#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

Assembly::Assembly(ScriptSystem* owner, MonoAssembly* handle) : Owner(owner), Handle(handle), Image(mono_assembly_get_image(handle))
{
	AddInternalCalls();
}

unique_ptr<Class> Assembly::InstantiateClass(const char* namespaceName, const char* className)
{
	MonoImage* image = mono_assembly_get_image(Handle);
	MonoClass* klass = mono_class_from_name(image, namespaceName, className);

	if(!klass)
	{
		spdlog::warn("Failed to load C# class '{}.{}'", namespaceName, className);
		return nullptr;
	}

	MonoObject* instance = mono_object_new(Owner->GetAppDomain(), klass);

	if(!instance)
	{
		spdlog::error("Failed to create instance of '{}.{}'", namespaceName, className);
		return nullptr;
	}

	// Call constructor
	mono_runtime_object_init(instance);

	return make_unique<Class>(klass, instance);
}

namespace InternalCalls
{
	extern void AddLogInternalCalls();
	extern void AddTimeInternalCalls();
	extern void AddVectorInternalCalls();
	extern void AddTransformInternalCalls();
	extern void AddWorldInternalCalls();
}

void Assembly::AddInternalCalls()
{
	InternalCalls::AddLogInternalCalls();
	InternalCalls::AddTimeInternalCalls();
	InternalCalls::AddWorldInternalCalls();
	InternalCalls::AddVectorInternalCalls();
	InternalCalls::AddTransformInternalCalls();
}
