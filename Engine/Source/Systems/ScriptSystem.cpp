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
	if (!ManagedData.Instance)
		return;

	MonoException* exception = nullptr;
	SystemMethodInitialise(ManagedData.Instance, GetWorldID(), &exception);
	SystemMethodStart(ManagedData.Instance, &exception);
}

void ScriptSystem::Destroy()
{
	if (!ManagedData.Instance)
		return;

	MonoException* exception = nullptr;
	SystemMethodDestroyed(ManagedData.Instance, &exception);
}

void ScriptSystem::OnEnabled()
{
	if (!ManagedData.Instance)
		return;

	MonoException* exception = nullptr;
	SystemMethodEnabled(ManagedData.Instance, true, &exception);
}

void ScriptSystem::OnDisabled()
{
	if (!ManagedData.Instance)
		return;

	MonoException* exception = nullptr;
	SystemMethodEnabled(ManagedData.Instance, false, &exception);
}

void ScriptSystem::Update()
{
	if (!ManagedData.Instance || !ManagedData.ShouldSendMessages)
		return;
	
	MonoException* exception = nullptr;
	SystemMethodUpdate(ManagedData.Instance, &exception);
	if (exception)
		mono_raise_exception(exception);
}

void ScriptSystem::Draw()
{
	if (!ManagedData.Instance || !ManagedData.ShouldSendMessages)
		return;

	MonoException* exception = nullptr;
	SystemMethodDraw(ManagedData.Instance, &exception);
}
