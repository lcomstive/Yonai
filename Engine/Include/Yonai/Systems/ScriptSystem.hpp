#pragma once
#include <mono/jit/jit.h>
#include <Yonai/Systems/System.hpp>

namespace Yonai::Systems
{
	struct ScriptSystem : public System
	{
		MonoType* Type = nullptr;

	protected:
		YonaiAPI void Init() override;
		YonaiAPI void Update() override;
		YonaiAPI void Draw() override;
		YonaiAPI void Destroy() override;

		YonaiAPI void OnEnabled() override;
		YonaiAPI void OnDisabled() override;

		YonaiAPI void OnScriptingReloadedAfter() override;
		YonaiAPI void OnScriptingReloadedBefore() override;

	private:
		UUID GetWorldID();
	};
}