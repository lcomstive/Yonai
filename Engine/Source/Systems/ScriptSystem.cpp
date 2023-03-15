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

#define TRACE_SYSTEM(msg) \
	World* world = GetWorld(); \
	spdlog::trace(msg " system '{}' [{}]", GetTypeName(ManagedData.Type), (world ? ("World #" + std::to_string(world->ID())) : "Global"));

unsigned int ScriptSystem::GetWorldID()
{
	World* world = GetWorld();
	return world ? world->ID() : InvalidEntityID;
}

char* GetTypeName(MonoType* type) { return mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_REFLECTION); }

void ScriptSystem::Init() { TRACE_SYSTEM("Initialising") }

void ScriptSystem::Destroy()
{
	TRACE_SYSTEM("Destroying")

	if (!ManagedData.IsValid())
		return;

	MonoException* exception = nullptr;
	SystemMethodDestroyed(ManagedData.GetInstance(), &exception);
}

void ScriptSystem::OnEnabled()
{
	TRACE_SYSTEM("Enabling")

	if (!ManagedData.IsValid())
		return;

	MonoException* exception = nullptr;
	SystemMethodEnabled(ManagedData.GetInstance(), true, &exception);
}

void ScriptSystem::OnDisabled()
{
	TRACE_SYSTEM("Disabling")

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

	TRACE_SYSTEM("Reloading")

	MonoObject* instance = ManagedData.GetInstance();
	MonoException* exception = nullptr;
	SystemMethodInitialise(instance, GetWorldID(), &exception);
	SystemMethodStart(instance, &exception);
}
