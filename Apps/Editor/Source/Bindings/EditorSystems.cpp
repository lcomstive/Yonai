#include <string>
#include <YonaiEditor/EditorApp.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Systems/ScriptSystem.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace std;
using namespace Yonai;
using namespace YonaiEditor;
using namespace Yonai::Scripting;

// Defined in Yonai/Engine/Source/Scripting/InternalCalls/World.cpp
extern size_t GetSystemType(MonoReflectionType* componentType);

ADD_MANAGED_METHOD(EditorSystems, _Has, bool, (MonoReflectionType* systemType), YonaiEditor)
{
	return EditorApp::GetSystemManager().Has(GetSystemType(systemType));
}

ADD_MANAGED_METHOD(EditorSystems, _Get, MonoObject*, (MonoReflectionType* systemType), YonaiEditor)
{
	MonoType* managedType = mono_reflection_type_get_type(systemType);
	MonoClass* klass = mono_type_get_class(managedType);
	size_t type = Assembly::GetTypeHash(mono_class_get_type(klass));

	Systems::System* instance = EditorApp::GetSystemManager().Get(type);
	return instance ? instance->ManagedData.GetInstance() : nullptr;
}

ADD_MANAGED_METHOD(EditorSystems, _GetAll, MonoArray*, (), YonaiEditor)
{
	vector<Systems::System*> systems = EditorApp::GetSystemManager().All();
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_object_class(), systems.size());
	for (size_t i = 0; i < systems.size(); i++)
		mono_array_set(output, MonoObject*, i, systems[i]->ManagedData.GetInstance());
	return output;
}

ADD_MANAGED_METHOD(EditorSystems, _Add, MonoObject*, (MonoReflectionType* systemType), YonaiEditor)
{
	SystemManager& systemManager = EditorApp::GetSystemManager();

	MonoType* managedType = mono_reflection_type_get_type(systemType);
	size_t typeHash = Assembly::GetTypeHash(managedType);

	// Check for existing component of same type on entity
	if (systemManager.Has(typeHash))
	{
		Yonai::Systems::System* system = systemManager.Get(typeHash);
		return system->ManagedData.GetInstance();
	}

	// Get unmanaged->managed data
	Assembly::ManagedSystemData systemData = Assembly::GetManagedSystemData(typeHash);
	if (systemData.AddFn == nullptr)
		return systemManager.Add(managedType)->ManagedData.GetInstance();
	return systemData.AddFn(&systemManager)->ManagedData.GetInstance();
}

ADD_MANAGED_METHOD(EditorSystems, _Remove, bool, (MonoReflectionType* systemType), YonaiEditor)
{
	return EditorApp::GetSystemManager().Remove(GetSystemType(systemType));
}

ADD_MANAGED_METHOD(EditorSystems, _Enable, void, (MonoReflectionType* systemType, bool enable), YonaiEditor)
{
	size_t type = GetSystemType(systemType);
	Systems::System* system = EditorApp::GetSystemManager().Get(type);
	if (!system || system->IsEnabled() == enable)
		return;

	system->Enable(enable);
}