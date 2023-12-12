#pragma once
#include <memory>
#include <functional>
#include <mono/jit/jit.h>
#include <Yonai/API.hpp>
#include <Yonai/Systems/System.hpp>
#include <Yonai/IO/FileWatcher.hpp>
#include <Yonai/Scripting/Assembly.hpp>

namespace Yonai::Scripting
{
	class ScriptEngine
	{
		static std::string s_CoreDLLPath;
		static bool s_IsReloading;
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

		// Loads YonaiScriptCore from <see cref="s_CoreDLLPath"/>
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
		/// <param name="assembliesPath">The path containing YonaiScriptCore.dll and Mono .dlls</param>
		/// <param name="allowDebugging">Should a debugger for managed code be setup?</param>
		YonaiAPI static void Init(std::string assembliesPath, bool allowDebugging = false);

		YonaiAPI static void AddInternalCall(const char* name, const void* fn);
		YonaiAPI static void AddInternalCalls(const std::vector<std::pair<const char*, const void*>>& methods);

		/// <summary>
		/// Unloads scripting engine, releases resources &
		/// clears all managed instances of components
		/// </summary>
		YonaiAPI static void Destroy();

		/// <returns>True if <see cref="Init" /> has been successfully called</returns>
		YonaiAPI static bool IsLoaded();

		/// <summary>
		/// Loads an assembly from disk
		/// </summary>
		/// <returns>Assembly from disk located at <paramref name="path"/>, or nullptr if failed to load</returns>
		YonaiAPI static Assembly* LoadAssembly(std::string path, bool shouldWatch =
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
		YonaiAPI static Assembly* LoadAssembly(std::vector<unsigned char>& assemblyData, const char* friendlyName);

		/// <returns>All assemblies currently loaded</summary>
		YonaiAPI static std::vector<Assembly*>& GetAssemblies();

		/// <returns>The assembly containing YonaiScriptCore</returns>
		YonaiAPI static Assembly* GetCoreAssembly();

		/// <summary>
		/// Tries to find a matching class across all currently loaded assemblies
		/// </summary>
		/// <param name="useClassCache">When true, uses the hash of the namespace and class to check against all cached types</param>
		/// <returns>Found class, or nullptr if not found</returns>
		YonaiAPI static MonoClass* TryGetClass(const char* _namespace, const char* _class, bool useClassCache = true);

		/// <returns>
		/// The currently loaded app domain,
		/// or nullptr if <see cref="Init"/> has not been called
		/// </returns>
		YonaiAPI static MonoDomain* GetAppDomain();

		/// <returns>True if Init() was called with debugging enabled</returns>
		YonaiAPI static bool DebuggingEnabled();

		/// <summary>
		/// Unloads & reloads all loaded assemblies.
		/// </summary>
		YonaiAPI static void Reload(bool force = false);

		/// <returns>True if a loaded assembly file was modified on disk</returns>
		YonaiAPI static bool AwaitingReload();

		/// <returns>True if assemblies are currently getting reloaded</returns>
		YonaiAPI static bool IsReloading();

		/// <returns>Inform the engine to reload when next avaialable</returns>
		YonaiAPI static void SetAwaitingReload();

		/// <returns>The managed type with matching hash, or nullptr if not found in any loaded assembly</returns>
		YonaiAPI static MonoType* GetTypeFromHash(size_t hash);

		/// <summary>
		/// Submit a callback to be invoked whenever the script engine and assemblies are reloaded
		/// </summary>
		YonaiAPI static void AddReloadCallback(std::function<void()> callback);

		/// <summary>
		/// Submit a callback to be invoked before the script engine and assemblies get unloaded and reloaded
		/// </summary>
		YonaiAPI static void AddPreReloadCallback(std::function<void()> callback);

		/// <returns>True if the assembly has been loaded via LoadAssembly</returns>
		YonaiAPI static bool IsAssemblyLoaded(std::string path);
	};
}