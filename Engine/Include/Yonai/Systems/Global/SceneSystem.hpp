#pragma once
#include <vector>
#include <functional>
#include <mono/jit/jit.h>
#include <Yonai/Systems/System.hpp>

namespace Yonai::Systems
{
	typedef void(*SceneCallback)(World*, bool added);

	class SceneSystem : public System
	{
		static MonoMethod* SceneChangedMethod;
		
		/// <summary>
		/// All currently loaded scenes
		/// </summary>
		static std::vector<World*> s_ActiveScenes;

		/// <summary>
		/// Callback listeners for when scenes are added & removed
		/// </summary>
		static std::vector<SceneCallback> s_SceneCallbacks;

		static SceneSystem* s_Instance;

		static void OnScriptEnginePreReloaded();
		static void OnScriptEngineReloaded();

		// Scene change callback, used to call a function in C#
		static void ManagedSceneCallback(World* world, bool added);

	public:
		YonaiAPI void Init() override;
		YonaiAPI void Draw() override;
		YonaiAPI void Update() override;
		YonaiAPI void Destroy() override;

		/// <summary>
		/// Clears all current scenes & loads a new scene
		/// </summary>
		YonaiAPI static void LoadScene(World* scene);

		/// <summary>
		/// Adds a scene, alongsdide already loaded scenes
		/// </summary>
		YonaiAPI static void AddScene(World* scene);

		/// <summary>
		/// Removes a scene being active
		/// </summary>
		YonaiAPI static void UnloadScene(int index);

		/// <summary>
		/// Removes a scene being active
		/// </summary>
		YonaiAPI static void UnloadScene(World* scene);

		/// <summary>
		/// Unloads all scenes from being active
		/// </summary>
		YonaiAPI static void UnloadAllScenes();
		
		/// <summary>
		/// Gets all actively loaded scenes
		/// </summary>
		YonaiAPI static std::vector<World*>& GetActiveScenes();

		/// <summary>
		/// Adds a listener for when scenes are added & removed
		/// </summary>
		YonaiAPI static void AddSceneCallback(SceneCallback callback);

		/// <summary>
		/// Removes a listener for when scenes are added & removed
		/// </summary>
		YonaiAPI static void RemoveSceneCallback(SceneCallback callback);
	};
}