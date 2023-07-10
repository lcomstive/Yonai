#include <spdlog/spdlog.h>
#include <Yonai/Scripting/Class.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Scripting;

Class::Class(MonoClass* handle, MonoObject* instance) : Handle(handle), Instance(instance) { }

MonoMethod* Class::GetMonoMethod(const char* name, int parameterCount)
{ return mono_class_get_method_from_name(Handle, name, parameterCount); }

Method Class::GetMethod(const char* name, int parameterCount)
{ return Method(GetMonoMethod(name, parameterCount)); }

MonoClassField* Class::GetField(const char* name)
{ return mono_class_get_field_from_name(Handle, name); }

MonoProperty* Class::GetProperty(const char* name)
{ return mono_class_get_property_from_name(Handle, name); }

void Class::Invoke(MonoMethod* method, void** params)
{ Method(method).Invoke(Instance, params); }

void Class::Invoke(const char* methodName, void** params) { Invoke(GetMonoMethod(methodName), params); }