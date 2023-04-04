#include <memory>
#include <AquaEngine/World.hpp>
#include <AquaEngine/ComponentManager.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/Component.hpp>
#include <AquaEngine/Components/ScriptComponent.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Components;

unordered_map<UUID, World*> World::s_Worlds;

World::World(string name) : m_Name(name)
{
	s_Worlds.emplace(m_ID, this);

	m_SystemManager = make_unique<AquaEngine::SystemManager>(this);
	m_ComponentManager = make_unique<AquaEngine::ComponentManager>(m_ID);
}

UUID& World::ID() { return m_ID; }
string& World::Name() { return m_Name; }
void World::Name(string& name) { m_Name = name; }

void World::Destroy()
{
	m_ComponentManager->Destroy();
	m_ComponentManager = nullptr;

	m_SystemManager->Destroy();

	s_Worlds.erase(m_ID);
}

void World::Update()
{
	m_SystemManager->Update();
	m_ComponentManager->CallUpdateFn();
}

Entity World::CreateEntity()
{
	Entity e(this);
	m_Entities.emplace(e.ID(), e);
	return e;
}

Entity World::CreateEntity(EntityID ID)
{
	Entity e(ID, this);
	m_Entities.emplace(ID, e);
	return e;
}

size_t World::EntityCount() { return m_Entities.size(); }

vector<Entity> World::Entities()
{
	vector<Entity> entities;
	for (auto pair : m_Entities)
		entities.push_back(pair.second);
	return entities;
}

bool World::HasEntity(EntityID entity)
{ return m_Entities.find(entity) != m_Entities.end(); }

Entity World::GetEntity(EntityID entity)
{ return m_Entities.find(entity) != m_Entities.end() ? m_Entities.at(entity) : Entity(); }

void World::DestroyEntity(EntityID entity)
{
	if(m_Entities.find(entity) != m_Entities.end())
		m_Entities.erase(entity);
}

void* World::GetComponent(EntityID entity, size_t type)
{ return m_ComponentManager->Get(entity, type); }

void World::OnActiveStateChanged(bool isActive)
{
	spdlog::trace("World '{}' active state changed to {}", Name(), isActive ? "active" : "inactive");
	m_ComponentManager->OnWorldActiveStateChanged(isActive);

	m_SystemManager->Enable(isActive);
}

ScriptComponent* World::AddComponent(EntityID entity, MonoType* managedType)
{
	ScriptComponent* instance = m_ComponentManager->Add(entity, managedType);
	SetupEntityComponent(entity, instance);
	return instance;
}
void World::RemoveComponent(EntityID entity, MonoType* managedType) { m_ComponentManager->Remove(entity, Scripting::Assembly::GetTypeHash(managedType)); }
bool World::HasComponent(EntityID entity, MonoType* managedType) { return m_ComponentManager->Has(entity, Scripting::Assembly::GetTypeHash(managedType)); }
ScriptComponent* World::GetComponent(EntityID entity, MonoType* managedType) { return (ScriptComponent*)m_ComponentManager->Get(entity, Scripting::Assembly::GetTypeHash(managedType)); }

bool World::HasComponents(EntityID entity) { return !m_ComponentManager->IsEmpty(entity); }
void World::SetupEntityComponent(EntityID id, Component* component) { component->Entity = GetEntity(id); }

SystemManager* World::GetSystemManager() { return m_SystemManager.get(); }
ComponentManager* World::GetComponentManager() { return m_ComponentManager.get(); }

void World::ClearComponents(EntityID entity) { m_ComponentManager->Clear(entity); }

World* World::GetWorld(UUID id)
{ return s_Worlds.find(id) == s_Worlds.end() ? nullptr : s_Worlds[id]; }

bool World::Exists(UUID id) { return s_Worlds.find(id) != s_Worlds.end(); }

vector<World*> World::GetWorlds()
{
	vector<World*> worlds = {};
	for (auto pair : s_Worlds)
		worlds.push_back(pair.second);
	return worlds;
}

#pragma region World::Entity
EntityID World::Entity::ID() { return m_ID; }
World* World::Entity::GetWorld() { return m_World; }
bool World::Entity::HasComponents() { return m_World ? m_World->HasComponents(m_ID) : false; }

void World::Entity::Destroy()
{
	// Inform world
	m_World->DestroyEntity(m_ID);
	
	// Invalidate
	m_World = nullptr;
}

ScriptComponent* Entity::AddComponent(MonoType* managedType) { return m_World ? m_World->AddComponent(m_ID, managedType) : nullptr; }
ScriptComponent* Entity::GetComponent(MonoType* managedType) { return m_World ? m_World->GetComponent(m_ID, managedType) : nullptr; }
bool Entity::HasComponent(MonoType* managedType) { return m_World ? m_World->HasComponent(m_ID, managedType) : false; }
void Entity::RemoveComponent(MonoType* managedType)
{
	if (m_World)
		m_World->RemoveComponent(m_ID, managedType);
}
#pragma endregion

void World::Entity::ClearComponents() { if (m_World) m_World->ClearComponents(m_ID); }
bool World::Entity::IsValid() { return m_World != nullptr; }