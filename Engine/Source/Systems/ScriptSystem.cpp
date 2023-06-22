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
	spdlog::trace(msg " system '{}' [{}]", GetTypeName(ManagedData.Type), (GetWorld() ? ("World #" + std::to_string(GetWorld()->ID())) : "Global"));

UUID ScriptSystem::GetWorldID()
{
	World* world = GetWorld();
	return world ? world->ID() : (UUID)0;
}

char* GetTypeName(MonoType* type) { return mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_REFLECTION); }

void ScriptSystem::Init()
{
	if (!ManagedData.IsValid())
		return;

	try
	{
		TRACE_SYSTEM("Initialising");

		MonoObject* instance = ManagedData.GetInstance();
		MonoException* exception = nullptr;
		SystemMethodInitialise(instance, GetWorldID(), &exception);
		if (exception)
			mono_print_unhandled_exception((MonoObject*)exception);

		TRACE_SYSTEM("Starting");
		SystemMethodStart(instance, &exception);
		if (exception)
			mono_print_unhandled_exception((MonoObject*)exception);
	}
	catch (std::exception& e)
	{
		spdlog::error("Failed to initialise system '{}' - {}", GetTypeName(Type), e.what());
		SystemManager* systemManager = GetWorld() ? GetWorld()->GetSystemManager() : SystemManager::Global();
		systemManager->Remove(Type);
	}
}

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
	if (!ManagedData.IsValid())
		return;
	TRACE_SYSTEM("Enabling")

	MonoException* exception = nullptr;
	SystemMethodEnabled(ManagedData.GetInstance(), true, &exception);
}

void ScriptSystem::OnDisabled()
{
	if (!ManagedData.IsValid())
		return;
	TRACE_SYSTEM("Disabling")

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
	{
		TRACE_SYSTEM("Exception in Update() for");
		mono_print_unhandled_exception((MonoObject*)exception);
		Enable(false);
	}
}

void ScriptSystem::Draw()
{
	if (!ManagedData.IsValid() || !ManagedData.ShouldSendMessages)
		return;

	MonoException* exception = nullptr;
	SystemMethodDraw(ManagedData.GetInstance(), &exception);
}

void ScriptSystem::OnScriptingReloadedBefore()
{
	if (!ManagedData.IsValid())
		return;

	TRACE_SYSTEM("OnScriptingReloadedBefore");
	
	MonoObject* instance = ManagedData.GetInstance();
	MonoException* exception = nullptr;

	// Disable
	SystemMethodEnabled(instance, false, &exception);
	// SystemMethodDestroyed(instance, &exception);
}

void ScriptSystem::OnScriptingReloadedAfter()
{
	if (!ManagedData.IsValid())
		return;

	TRACE_SYSTEM("OnScriptingReloadedAfter");
	
	MonoObject* instance = ManagedData.GetInstance();
	MonoException* exception = nullptr;

	// Enable
	SystemMethodEnabled(instance, true, &exception);
	// SystemMethodStart(instance, &exception);
}
