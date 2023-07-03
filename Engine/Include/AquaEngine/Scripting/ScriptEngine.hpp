#pragma once
#include <memory>
#include <functional>
#include <mono/jit/jit.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/IO/FileWatcher.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>

namespace AquaEngine::Scripting
{
	class ScriptEngine
	{
		static std::string s_CoreDLLPath;
		static bool s_AwaitingReload;
		static bool s_DebuggingEnabled;
		static MonoDomain* s_AppDomain;
		static MonoDomain* s_RootDomain;
		static Assembly* s_CoreAssembly;
		static std::vector<Assembly*> s_Assemblies;
		static std::vector<IO::FileWatcher*> s_FileWatchers;

		// Callbacks when script engine reloads all assemblies
		static std::vector<std::function<void()>> s_ReloadCallbacks;

		// Callbacks when script engine is about to reloads
		static std::vector<std::function<void()>> s_PreReloadCallbacks;

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
		/// <param name="assembliesPath">The path containing AquaScriptCore.dll and Mono .dlls</param>
		/// <param name="allowDebugging">Should a debugger for managed code be setup?</param>
		AquaAPI static void Init(std::string assembliesPath, bool allowDebugging = false);

		AquaAPI static void AddInternalCall(const char* name, const void* fn);
		AquaAPI static void AddInternalCalls(const std::vector<std::pair<const char*, const void*>>& methods);

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
		/// <returns>Assembly from disk located at <paramref name="path"/>, or nullptr if failed to load</returns>
		AquaAPI static Assembly* LoadAssembly(std::string path, bool shouldWatch =
#ifndef NDEBUG
			// Debug mode, watch files by default
			true
#else
			// Release mode, don't watch files by default
			false
#endif
		);

		/// <summary>
		/// Loads an assembly from memory
		/// </summary>
		/// <param name="friendlyName">Name for debugging, warnings and errors</param>
		AquaAPI static Assembly* LoadAssembly(std::vector<unsigned char>& assemblyData, const char* friendlyName);


		/// <returns>All assemblies currently loaded</summary>
		AquaAPI static std::vector<Assembly*>& GetAssemblies();

		/// <returns>The assembly containing AquaScriptCore</returns>
		AquaAPI static Assembly* GetCoreAssembly();

		/// <returns>
		/// The currently loaded app domain,
		/// or nullptr if <see cref="Init"/> has not been called
		/// </returns>
		AquaAPI static MonoDomain* GetAppDomain();

		/// <returns>True if Init() was called with debugging enabled</returns>
		AquaAPI static bool DebuggingEnabled();

		/// <summary>
		/// Unloads & reloads all loaded assemblies.
		/// </summary>
		AquaAPI static void Reload(bool force = false);

		/// <returns>True if a loaded assembly file was modified on disk</returns>
		AquaAPI static bool AwaitingReload();

		/// <returns>Inform the engine to reload when next avaialable</returns>
		AquaAPI static void SetAwaitingReload();

		/// <returns>The managed type with matching hash, or nullptr if not found in any loaded assembly</returns>
		AquaAPI static MonoType* GetTypeFromHash(size_t hash);

		/// <summary>
		/// Submit a callback to be invoked whenever the script engine and assemblies are reloaded
		/// </summary>
		AquaAPI static void AddReloadCallback(std::function<void()> callback);

		/// <summary>
		/// Submit a callback to be invoked before the script engine and assemblies get unloaded and reloaded
		/// </summary>
		AquaAPI static void AddPreReloadCallback(std::function<void()> callback);

		/// <returns>True if the assembly has been loaded via LoadAssembly</returns>
		AquaAPI static bool IsAssemblyLoaded(std::string path);
	};
}