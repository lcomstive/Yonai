#include <Yonai/World.hpp>
#include <Yonai/SystemManager.hpp>
#include <Yonai/Systems/System.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace Yonai;
using namespace Yonai::Systems;
using namespace Yonai::Scripting;

bool System::IsEnabled() { return m_Enabled; }

void System::ToggleEnabled() { Enable(!m_Enabled); }

void System::Enable(bool enable)
{
	if (m_Enabled == enable)
		return; // Already in desired state
	if (enable)
		OnEnabled();
	else
		OnDisabled();
	m_Enabled = enable;
}

World* System::GetWorld() { return m_Owner ? m_Owner->GetWorld() : nullptr; }
SystemManager* System::GetManager() { return m_Owner; }

ADD_MANAGED_METHOD(NativeSystem, GetHandle, void*, (unsigned int worldID, MonoReflectionType* type))
{
	SystemManager* systemManager = nullptr;

	if (worldID != InvalidEntityID)
	{
		World* world = World::GetWorld(worldID);
		if (!world)
			return nullptr;
		systemManager = world->GetSystemManager();
	}
	else
		systemManager = SystemManager::Global();

	size_t typeHash = Assembly::GetTypeHash(mono_reflection_type_get_type(type));
	void* result = systemManager->Get(typeHash);
	return result;
}