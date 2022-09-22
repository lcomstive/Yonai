#pragma once
#include <AquaEngine/API.hpp>
	
namespace AquaEngine { class World; }

namespace AquaEngine::Systems
{
	class System
	{
	private:
		bool m_Enabled = true;

	public:
#pragma region Virtual Functions
		AquaAPI virtual void Init() { }
		AquaAPI virtual void Update() { }
		AquaAPI virtual void Draw() { }
		AquaAPI virtual void Destroy() { }

		AquaAPI virtual void OnEnabled() { }
		AquaAPI virtual void OnDisabled() { }
#pragma endregion

		AquaAPI bool IsEnabled();

		AquaAPI void ToggleEnabled();

		AquaAPI void Enable(bool enable = true);
	};
}