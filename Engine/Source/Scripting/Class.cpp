#include <AquaEngine/Scripting/Class.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Scripting;

Class::Class(MonoClass* handle, MonoObject* instance) : Handle(handle), Instance(instance) { }

unique_ptr<Method> Class::GetMethod(const char* name, int parameterCount)
{ return make_unique<Method>(Instance, mono_class_get_method_from_name(Handle, name, parameterCount)); }

MonoClassField* Class::GetField(const char* name)
{ return mono_class_get_field_from_name(Handle, name); }

MonoProperty* Class::GetProperty(const char* name)
{ return mono_class_get_property_from_name(Handle, name); }