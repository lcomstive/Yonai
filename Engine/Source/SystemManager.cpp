#include <iostream>
#include <AquaEngine/World.hpp>
#include <AquaEngine/SystemManager.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

SystemManager* SystemManager::s_Global = nullptr;

SystemManager::SystemManager(World* owner) : m_Owner(owner) { }

void SystemManager::Init()
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

	if(this == s_Global)
	{
		delete this;
		s_Global = nullptr;
	}
}

World* SystemManager::GetWorld() { return m_Owner; }

SystemManager* SystemManager::Global()
{
	if(!s_Global)
		s_Global = new SystemManager(nullptr);
	return s_Global;
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