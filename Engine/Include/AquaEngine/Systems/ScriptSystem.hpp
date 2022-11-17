#pragma once
#include <mono/jit/jit.h>
#include <AquaEngine/Systems/System.hpp>

namespace AquaEngine::Systems
{
	struct ScriptSystem : public System
	{
		MonoType* Type = nullptr;

	protected:
		AquaAPI void Init() override;
		AquaAPI void Update() override;
		AquaAPI void Draw() override;
		AquaAPI void Destroy() override;

		AquaAPI void OnEnabled() override;
		AquaAPI void OnDisabled() override;

		AquaAPI void OnScriptingReloaded() override;

	private:
		unsigned int GetWorldID();
	};
}