#include <algorithm>
#include <Yonai/Time.hpp>
#include <Yonai/World.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Systems;
using namespace Yonai::Scripting;

SceneSystem* SceneSystem::s_Instance = nullptr;
vector<World*> SceneSystem::s_ActiveScenes = {};
vector<SceneCallback> SceneSystem::s_SceneCallbacks = {};
MonoMethod* SceneSystem::SceneChangedMethod = nullptr;

void SceneSystem::Init()
{
	s_Instance = this;

	// Cache scene manager class
	OnScriptEngineReloaded();

	// Add callback for when script engine is about to get reloaded
	ScriptEngine::AddPreReloadCallback(OnScriptEnginePreReloaded);

	// Add callback for when script engine gets reloaded
	ScriptEngine::AddReloadCallback(OnScriptEngineReloaded);
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

void SceneSystem::OnScriptEnginePreReloaded()
{
	// UnloadAllScenes();
	SceneChangedMethod = nullptr;
}

void SceneSystem::OnScriptEngineReloaded()
{
	if (!ScriptEngine::IsLoaded())
		return;
	
	MonoClass* managedClass = ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai", "SceneManager");
	SceneChangedMethod = mono_class_get_method_from_name(
		managedClass,
		"_UpdateScenes", // Function name
		0 // Parameter count
	);

	mono_runtime_invoke(SceneChangedMethod, nullptr, nullptr, nullptr);
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
	for (size_t i = 0; i <= s_ActiveScenes.size(); i++)
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

	spdlog::debug("Unloaded scene, remaining: {}", (int)s_ActiveScenes.size());
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

ADD_MANAGED_METHOD(SceneManager, Load, void, (uint64_t worldID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		SceneSystem::LoadScene(world);
}

ADD_MANAGED_METHOD(SceneManager, LoadAdditive, void, (uint64_t worldID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		SceneSystem::AddScene(world);
}

ADD_MANAGED_METHOD(SceneManager, Unload, void, (uint64_t worldID))
{
	World* world = World::GetWorld(worldID);
	if (world)
		SceneSystem::UnloadScene(world);
	else
		spdlog::warn("[SceneManager.Unload] Could not find world with ID '{}'", worldID);
}

ADD_MANAGED_METHOD(SceneManager, UnloadAll)
{ SceneSystem::UnloadAllScenes(); }

ADD_MANAGED_METHOD(SceneManager, GetActiveScenes, MonoArray*)
{
	vector<World*> scenes = SceneSystem::GetActiveScenes();
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_uint64_class(), scenes.size());

	for (size_t i = 0; i < scenes.size(); i++)
		mono_array_set(output, uint64_t, i, scenes[i]->ID());

	return output;
}