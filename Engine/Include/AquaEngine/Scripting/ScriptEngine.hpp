#pragma once
#include <memory>
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Systems/System.hpp>

namespace AquaEngine::Scripting
{
	// Forward declaration
	struct Assembly;

	class ScriptEngine
	{
		std::string m_CoreDLLPath;
		bool m_AwaitingReload = false;
		MonoDomain* m_AppDomain = nullptr;
		MonoDomain* m_RootDomain = nullptr;
		std::vector<Assembly*> m_Assemblies;

#ifndef NDEBUG
		// Paths to all loaded assemblies
		std::vector<std::string> m_AssemblyPaths;

		void OnAssemblyFileChanged(const std::string& path, IO::FileWatchStatus status);
#endif

		void LoadCoreAssembly();
		Assembly* LoadAssembly(std::string& path, bool watch);

	public:
		AquaAPI ScriptEngine(std::string& coreDllPath);
		AquaAPI ~ScriptEngine();

		AquaAPI Assembly* LoadAssembly(std::string path);

		AquaAPI MonoDomain* GetAppDomain();

		AquaAPI void Reload(bool force = false);
		AquaAPI bool AwaitingReload();
	};
}