#include <iostream>
#include <AquaEngine/World.hpp>
#include <AquaEngine/SystemManager.hpp>

#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Systems/ScriptSystem.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

extern SystemMethodInitialiseFn SystemMethodInitialise;

SystemManager* SystemManager::s_Global = nullptr;

SystemManager::SystemManager(World* owner) : m_Owner(owner) { }

void SystemManager::Enable(bool enable)
{
	// Do initial Enabled calls
	for (auto& iterator : m_Systems)
	{
		if (!iterator.second->IsEnabled())
			continue;
		if (enable)
			iterator.second->OnEnabled();
		else
			iterator.second->OnDisabled();
	}
}

void SystemManager::Destroy()
{
	for (auto& iterator : m_Systems)
	{
		if(iterator.second->IsEnabled())
			iterator.second->OnDisabled();
		iterator.second->Destroy();
		delete iterator.second;
	}
	m_Systems.clear();

	if (this == s_Global)
	{
		delete this;
		s_Global = nullptr;
	}
}

World* SystemManager::GetWorld() { return m_Owner; }

SystemManager* SystemManager::Global()
{
	if (!s_Global)
		s_Global = new SystemManager(nullptr);
	return s_Global;
}

void SystemManager::Update()
{
	for (auto& pair : m_Systems)
		if (pair.second->IsEnabled())
			pair.second->Update();
}

void SystemManager::Draw()
{
	for (auto& pair : m_Systems)
		if (pair.second->IsEnabled())
			pair.second->Draw();
}

vector<System*> SystemManager::All()
{
	vector<System*> systems;
	for (auto& pair : m_Systems)
		systems.push_back(pair.second);
	return systems;
}

bool SystemManager::Has(size_t hash) { return m_Systems.find(hash) != m_Systems.end(); }

System* SystemManager::Get(size_t hash) { return Has(hash) ? m_Systems[hash] : nullptr; }

AquaEngine::Scripting::ManagedData SystemManager::CreateManagedInstance(size_t typeHash)
{
	MonoType* managedType = ScriptEngine::GetTypeFromHash(typeHash);
	Assembly::ManagedSystemData managedData = ScriptEngine::GetCoreAssembly()->GetManagedSystemData(typeHash);
	if (!managedType)
	{
		if (managedData.AddFn)
			return managedData.AddFn(this)->ManagedData;
		return {};
	}

	// Create managed (C#) instance
	MonoDomain* domain = mono_domain_get();
	MonoObject* instance = mono_object_new(mono_domain_get(), mono_class_from_mono_type(managedType));

	// Call constructor
	mono_runtime_object_init(instance);

	auto componentData = ScriptEngine::GetCoreAssembly()->GetManagedSystemData(typeHash);
	return {
		managedData.AddFn == nullptr,
		mono_gchandle_new(instance, false),
		managedType
	};
}

void SystemManager::InvalidateAllManagedInstances()
{
	for (auto pair : m_Systems)
	{
		if (!pair.second->ManagedData.IsValid())
			continue;
		mono_gchandle_free(pair.second->ManagedData.GCHandle);
		pair.second->ManagedData = {};
	}
}

void SystemManager::CreateAllManagedInstances()
{
	for (auto pair : m_Systems)
	{
		pair.second->ManagedData = CreateManagedInstance(pair.first);
		pair.second->OnScriptingReloaded();
	}
}

ScriptSystem* SystemManager::Add(MonoType* managedType)
{
	size_t typeHash = Scripting::Assembly::GetTypeHash(managedType);

	if (Has(typeHash))
		return (ScriptSystem*)m_Systems[typeHash];

	ScriptSystem* system = new ScriptSystem();
	((System*)system)->m_Owner = this;
	m_Systems.emplace(typeHash, system);

	// Set type in system
	system->Type = managedType;

	// Cache scripting side of system
	if (Scripting::ScriptEngine::IsLoaded())
		system->ManagedData = CreateManagedInstance(typeHash);
		
	System* rawSystem = (System*)system;
	rawSystem->Init();
	rawSystem->OnScriptingReloaded();

	return system;
}

bool SystemManager::Remove(size_t hash)
{
	if (!Has(hash))
		return false;
	
	// Free managed memory
	if (m_Systems[hash]->ManagedData.IsValid())
		mono_gchandle_free(m_Systems[hash]->ManagedData.GCHandle);

	// Free unmanaged memory
	delete m_Systems[hash];

	// Remove from map
	m_Systems.erase(hash);
	return true;
}