#include <algorithm>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Systems/SceneSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

void SceneSystem::Update()
{
	
}

void SceneSystem::LoadScene(World* scene)
{
	UnloadAllScenes();
	AddScene(scene);
}

void SceneSystem::AddScene(World* scene)
{
	m_ActiveScenes.emplace_back(scene);
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
