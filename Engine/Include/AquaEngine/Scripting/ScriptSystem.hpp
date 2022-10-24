#pragma once
#include <memory>
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/Systems/System.hpp>

namespace AquaEngine::Scripting
{
	// Forward declaration
	struct Assembly;

	class ScriptSystem : public Systems::System
	{
		MonoDomain* m_AppDomain = nullptr;
		MonoDomain* m_RootDomain = nullptr;

	public:
		AquaAPI ScriptSystem();
		AquaAPI ~ScriptSystem();

		AquaAPI std::unique_ptr<Assembly> LoadAssembly(std::string& path);

		AquaAPI MonoDomain* GetAppDomain();
	};
}