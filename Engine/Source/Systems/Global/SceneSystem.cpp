#include <algorithm>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

SceneSystem* SceneSystem::s_Instance = nullptr;
vector<World*> SceneSystem::m_ActiveScenes = {};
vector<SceneCallback> SceneSystem::m_SceneCallbacks = {};

void SceneSystem::Init() { s_Instance = this; }

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

void SceneSystem::LoadScene(World* scene)
{
	UnloadAllScenes();
	AddScene(scene);
}

void SceneSystem::AddScene(World* scene)
{
	m_ActiveScenes.emplace_back(scene);
	scene->OnActiveStateChanged(true);
	for (auto& callback : m_SceneCallbacks)
		callback(scene, true);
}

void SceneSystem::UnloadAllScenes()
{
	for (size_t i = 0; i < m_ActiveScenes.size(); i++)
		UnloadScene(0);
}

void SceneSystem::UnloadScene(World* scene)
{
	for (size_t i = 0; i < m_ActiveScenes.size(); i++)
	{
		if (m_ActiveScenes[i] != scene)
			continue;

		UnloadScene((int)i);
		break;
	}
}

void SceneSystem::UnloadScene(int index)
{
	if (m_ActiveScenes.empty())
		return;

	index = clamp(index, 0, (int)m_ActiveScenes.size());

	m_ActiveScenes[index]->OnActiveStateChanged(false);

	for (auto& callback : m_SceneCallbacks)
		callback(m_ActiveScenes[index], false);

	m_ActiveScenes.erase(m_ActiveScenes.begin() + index);
}

std::vector<World*>& SceneSystem::GetActiveScenes() { return m_ActiveScenes; }

void SceneSystem::AddSceneCallback(SceneCallback callback)
{
	m_SceneCallbacks.emplace_back(callback);
}

void SceneSystem::RemoveSceneCallback(SceneCallback callback)
{
	for (int i = 0; i < m_SceneCallbacks.size(); i++)
	{
		if (m_SceneCallbacks[i] != callback)
			continue;

		m_SceneCallbacks.erase(m_SceneCallbacks.begin() + i);
		break;
	}
}
