#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Class.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

Class::Class(MonoClass* handle, MonoObject* instance) : Handle(handle), Instance(instance) { }

MonoMethod* Class::GetMethod(const char* name, int parameterCount)
{ return mono_class_get_method_from_name(Handle, name, parameterCount); }

MonoClassField* Class::GetField(const char* name)
{ return mono_class_get_field_from_name(Handle, name); }

MonoProperty* Class::GetProperty(const char* name)
{ return mono_class_get_property_from_name(Handle, name); }

void Class::Invoke(MonoMethod* method, void** params)
{
	if (!method)
	{
		spdlog::warn("Tried to invoke method but no valid one was passed");
		return;
	}
	MonoObject* exception = nullptr;
	mono_runtime_invoke(method, Instance, params, &exception);

	if (exception)
		mono_print_unhandled_exception(exception);
}

void Class::Invoke(const char* methodName, void** params) { Invoke(GetMethod(methodName), params); }