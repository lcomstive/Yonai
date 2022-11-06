#pragma once
#include <AquaEngine/API.hpp>
#include <AquaEngine/Scripting/ManagedData.hpp>

// Forward declarations
struct _MonoType;
struct _MonoObject;
namespace AquaEngine { class World; class SystemManager; }

namespace AquaEngine::Systems
{	
	class System
	{
	private:
		bool m_Enabled = true;
		SystemManager* m_Owner = nullptr;

		friend class AquaEngine::SystemManager;

	protected:
#pragma region Virtual Functions
		AquaAPI virtual void Init() { }
		AquaAPI virtual void Update() { }
		AquaAPI virtual void Draw() { }
		AquaAPI virtual void Destroy() { }

		AquaAPI virtual void OnEnabled() { }
		AquaAPI virtual void OnDisabled() { }
#pragma endregion

	public:
		AquaEngine::Scripting::ManagedData ManagedData;

		AquaAPI bool IsEnabled();

		AquaAPI void ToggleEnabled();

		AquaAPI void Enable(bool enable = true);

		AquaAPI World* GetWorld();
		AquaAPI SystemManager* GetManager();
	};
}