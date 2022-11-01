#pragma once
#include <memory>
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>

namespace AquaEngine::Scripting
{
	class ScriptEngine
	{
		static std::string s_CoreDLLPath;
		static bool s_AwaitingReload;
		static MonoDomain* s_AppDomain;
		static MonoDomain* s_RootDomain;
		static Assembly* s_CoreAssembly;
		static std::vector<Assembly*> s_Assemblies;

		struct AssemblyPath
		{
			std::string Path;
			bool WatchForChanges;
		};

		// Paths to all loaded assemblies
		static std::vector<AssemblyPath> s_AssemblyPaths;

		// Callback 
		static void OnAssemblyFileChanged(const std::string& path, IO::FileWatchStatus status);

		// Loads AquaScriptCore from <see cref="s_CoreDLLPath"/>
		static void LoadCoreAssembly();

		/// <summary>
		/// Loads an assembly from disk
		/// </summary>
		/// <returns>Assembly from disk, located at <paramref name="path"/></returns>
		static Assembly* LoadAssembly(std::string& path, bool isCoreAssembly, bool shouldWatch);

	public:
		/// <summary>
		/// Begins the scripting engine and loads the core assembly
		/// </summary>
		/// <param name="coreDllPath">The path to AquaScriptCore.dll</param>
		/// <param name="allowDebugging">Should a debugger for managed code be setup?</param>
		AquaAPI static void Init(std::string& coreDllPath, bool allowDebugging = false);

		/// <summary>
		/// Unloads scripting engine, releases resources &
		/// clears all managed instances of components
		/// </summary>
		AquaAPI static void Destroy();

		/// <returns>True if <see cref="Init" /> has been successfully called</returns>
		AquaAPI static bool IsLoaded();

		/// <summary>
		/// Loads an assembly from disk
		/// </summary>
		/// <returns>Assembly from disk, located at <paramref name="path"/></returns>
		AquaAPI static Assembly* LoadAssembly(std::string path, bool shouldWatch =
#ifndef NDEBUG
			// Debug mode, watch files by default
			true
#else
			// Release mode, don't watch files by default
			false
#endif
		);

		/// <returns>All assemblies currently loaded</summary>
		AquaAPI static std::vector<Assembly*>& GetAssemblies();

		/// <returns>The assembly containing AquaScriptCore</returns>
		AquaAPI static Assembly* GetCoreAssembly();

		/// <returns>
		/// The currently loaded app domain,
		/// or nullptr if <see cref="Init"/> has not been called
		/// </returns>
		AquaAPI static MonoDomain* GetAppDomain();

		/// <summary>
		/// Unloads & reloads all loaded assemblies.
		/// </summary>
		AquaAPI static void Reload(bool force = false);

		/// <returns>True if a loaded assembly file was modified on disk</returns>
		AquaAPI static bool AwaitingReload();

		/// <returns>The managed type with matching hash, or nullptr if not found in any loaded assembly</returns>
		AquaAPI static MonoType* GetTypeFromHash(size_t hash);
	};
}