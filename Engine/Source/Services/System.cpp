#include <AquaEngine/World.hpp>
#include <AquaEngine/Systems/System.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Systems;

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