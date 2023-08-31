#pragma once
#include <Yonai/API.hpp>
#include <Yonai/Scripting/ManagedData.hpp>

// Forward declarations
struct _MonoType;
struct _MonoObject;
namespace Yonai { class World; class SystemManager; }

namespace Yonai::Systems
{	
	class System
	{
	private:
		bool m_Enabled = true;
		SystemManager* m_Owner = nullptr;

		friend class Yonai::SystemManager;

	protected:
#pragma region Virtual Functions
		YonaiAPI virtual void Init() { }
		YonaiAPI virtual void Update() { }
		YonaiAPI virtual void Draw() { }
		YonaiAPI virtual void Destroy() { }

		YonaiAPI virtual void OnEnabled() { }
		YonaiAPI virtual void OnDisabled() { }

		/// <summary>
		/// Called when the script engine is about to reload
		/// </summary>
		YonaiAPI virtual void OnScriptingReloadedBefore() { }

		/// <summary>
		/// Called after the script engine has finished reloading
		/// </summary>
		YonaiAPI virtual void OnScriptingReloadedAfter() { }
#pragma endregion

	public:
		Yonai::Scripting::ManagedData ManagedData;

		YonaiAPI bool IsEnabled();

		YonaiAPI void ToggleEnabled();

		YonaiAPI void Enable(bool enable = true);

		YonaiAPI World* GetWorld();
		YonaiAPI SystemManager* GetManager();
	};
}