#include <iostream>
#include <AquaEngine/SystemManager.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

unordered_map<type_index, System*> SystemManager::m_Systems = {};

void SystemManager::Initialize()
{
	for (auto& iterator : m_Systems)
		iterator.second->Init();

	// Do initial OnEnable/OnDisable calls
	for (auto& iterator : m_Systems)
	{
		if (iterator.second->IsEnabled())
			iterator.second->OnEnabled();
		else
			iterator.second->OnDisabled();
	}
}

void SystemManager::Destroy()
{
	for (auto& iterator : m_Systems)
	{
		iterator.second->Destroy();
		delete iterator.second;
	}
	m_Systems.clear();
}

void SystemManager::Update()
{
	for (auto& pair : m_Systems)
		if(pair.second->IsEnabled())
			pair.second->Update();
}

void SystemManager::Draw()
{
	for (auto& pair : m_Systems)
		if(pair.second->IsEnabled())
			pair.second->Draw();
}

vector<System*> SystemManager::All()
{
	vector<System*> systems;
	for (auto& pair : m_Systems)
		systems.push_back(pair.second);
	return systems;
}