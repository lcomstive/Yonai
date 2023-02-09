#include <imgui.h>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Systems/ScriptSystem.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

extern EmptyMethodFn SystemMethodDraw;
extern EmptyMethodFn SystemMethodStart;
extern EmptyMethodFn SystemMethodUpdate;
extern EmptyMethodFn SystemMethodDestroyed;
extern SystemMethodEnabledFn SystemMethodEnabled;
extern SystemMethodInitialiseFn SystemMethodInitialise;

unsigned int ScriptSystem::GetWorldID()
{
	World* world = GetWorld();
	return world ? world->ID() : InvalidEntityID;
}

void ScriptSystem::Init()
{
	spdlog::debug("ScriptSystem::Init");
}

void ScriptSystem::Destroy()
{
	spdlog::debug("ScriptSystem::Destroy");
	if (!ManagedData.IsValid())
		return;

	MonoException* exception = nullptr;
	SystemMethodDestroyed(ManagedData.GetInstance(), &exception);
}

void ScriptSystem::OnEnabled()
{
	spdlog::debug("ScriptSystem::OnEnabled ({})", ManagedData.GCHandle);
	if (!ManagedData.IsValid())
		return;

	MonoException* exception = nullptr;
	SystemMethodEnabled(ManagedData.GetInstance(), true, &exception);
}

void ScriptSystem::OnDisabled()
{
	spdlog::debug("ScriptSystem::OnDisabled ({})", ManagedData.GCHandle);
	if (!ManagedData.IsValid())
		return;

	MonoException* exception = nullptr;
	SystemMethodEnabled(ManagedData.GetInstance(), false, &exception);
}

void ScriptSystem::Update()
{
	if (!ManagedData.IsValid() || !ManagedData.ShouldSendMessages)
		return;
	
	MonoException* exception = nullptr;
	SystemMethodUpdate(ManagedData.GetInstance(), &exception);
	if (exception)
		mono_raise_exception(exception);
}

void ScriptSystem::Draw()
{
	if (!ManagedData.IsValid() || !ManagedData.ShouldSendMessages)
		return;

	MonoException* exception = nullptr;
	SystemMethodDraw(ManagedData.GetInstance(), &exception);
}

void ScriptSystem::OnScriptingReloaded()
{
	if (!ManagedData.IsValid())
	{
		spdlog::critical("Managed data is invalid on ScriptSystem?");
		return;
	}
	spdlog::debug("ScriptSystem::OnScriptingReloaded");

	MonoObject* instance = ManagedData.GetInstance();
	MonoException* exception = nullptr;
	SystemMethodInitialise(instance, GetWorldID(), &exception);
	SystemMethodStart(instance, &exception);
}
