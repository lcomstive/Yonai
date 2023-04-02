#include <algorithm>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

SceneSystem* SceneSystem::s_Instance = nullptr;
vector<World*> SceneSystem::s_ActiveScenes = {};
vector<SceneCallback> SceneSystem::s_SceneCallbacks = {};
MonoMethod* SceneSystem::s_SceneChangeManagedMethod = nullptr;

void SceneSystem::Init()
{
	s_Instance = this;

	// Cache scene manager class
	OnScriptEngineReloaded();

	// Add callback for when script engine gets reloaded
	ScriptEngine::AddReloadCallback(OnScriptEngineReloaded);

	// Add callback for scene changes, to inform C#
	AddSceneCallback(ManagedSceneCallback);
}

void SceneSystem::Destroy()
{
	if(this == s_Instance)
		s_Instance = nullptr;
}

void SceneSystem::Update()
{
	auto scenes = GetActiveScenes();
	for (auto& scene : scenes)
		scene->Update();
}

void SceneSystem::Draw()
{
	auto scenes = GetActiveScenes();
	for(auto& scene : scenes)
		scene->GetSystemManager()->Draw();
}

void SceneSystem::OnScriptEngineReloaded()
{
	if (ScriptEngine::IsLoaded())
		s_SceneChangeManagedMethod = mono_class_get_method_from_name(
			ScriptEngine::GetCoreAssembly()->GetClassFromName("AquaEngine", "SceneManager"),
			"_OnSceneChanged", // Function name
			2 // Parameter count
		);
	else
		s_SceneChangeManagedMethod = nullptr;
}

void SceneSystem::ManagedSceneCallback(World* world, bool added)
{
	if (!s_SceneChangeManagedMethod)
	{
		if (ScriptEngine::IsLoaded())
			OnScriptEngineReloaded();
		else
			return;
	}

	MonoObject* exception;
	void* args[2];
	args[0] = &world->m_ID;
	args[1] = &added;
	mono_runtime_invoke(s_SceneChangeManagedMethod, nullptr, args, &exception);
}

void SceneSystem::LoadScene(World* scene)
{
	UnloadAllScenes();
	AddScene(scene);
}

void SceneSystem::AddScene(World* scene)
{
	s_ActiveScenes.emplace_back(scene);
	scene->OnActiveStateChanged(true);
	for (auto& callback : s_SceneCallbacks)
		callback(scene, true);
}

void SceneSystem::UnloadAllScenes()
{
	for (size_t i = 0; i < s_ActiveScenes.size(); i++)
		UnloadScene(0);
}

void SceneSystem::UnloadScene(World* scene)
{
	for (size_t i = 0; i < s_ActiveScenes.size(); i++)
	{
		if (s_ActiveScenes[i] != scene)
			continue;

		UnloadScene((int)i);
		break;
	}
}

void SceneSystem::UnloadScene(int index)
{
	if (s_ActiveScenes.empty())
		return;

	index = clamp(index, 0, (int)s_ActiveScenes.size());

	s_ActiveScenes[index]->OnActiveStateChanged(false);

	// Invoke callbacks
	for (auto& callback : s_SceneCallbacks)
		callback(s_ActiveScenes[index], false);

	s_ActiveScenes.erase(s_ActiveScenes.begin() + index);
}

std::vector<World*>& SceneSystem::GetActiveScenes() { return s_ActiveScenes; }

void SceneSystem::AddSceneCallback(SceneCallback callback)
{
	s_SceneCallbacks.emplace_back(callback);
}

void SceneSystem::RemoveSceneCallback(SceneCallback callback)
{
	for (int i = 0; i < s_SceneCallbacks.size(); i++)
	{
		if (s_SceneCallbacks[i] != callback)
			continue;

		s_SceneCallbacks.erase(s_SceneCallbacks.begin() + i);
		break;
	}
}

ADD_MANAGED_METHOD(SceneManager, Load, void, (unsigned int worldID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		SceneSystem::LoadScene(world);
}

ADD_MANAGED_METHOD(SceneManager, LoadAdditive, void, (unsigned int worldID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		SceneSystem::AddScene(world);
}

ADD_MANAGED_METHOD(SceneManager, Unload, void, (unsigned int worldID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		SceneSystem::UnloadScene(world);
}

ADD_MANAGED_METHOD(SceneManager, UnloadAll)
{ SceneSystem::UnloadAllScenes(); }

ADD_MANAGED_METHOD(SceneManager, GetActiveScenes, MonoArray*)
{
	vector<World*> scenes = SceneSystem::GetActiveScenes();
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint32_class(), scenes.size());

	for (size_t i = 0; i < scenes.size(); i++)
		mono_array_set(output, unsigned int, i, scenes[i]->ID());

	return output;
}