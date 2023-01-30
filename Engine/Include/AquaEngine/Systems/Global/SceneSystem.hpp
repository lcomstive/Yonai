#pragma once
#include <vector>
#include <functional>
#include <AquaEngine/Systems/System.hpp>

namespace AquaEngine::Systems
{
	typedef void(*SceneCallback)(World*, bool added);

	class SceneSystem : public System
	{
		/// <summary>
		/// All currently loaded scenes
		/// </summary>
		static std::vector<World*> m_ActiveScenes;

		/// <summary>
		/// Callback listeners for when scenes are added & removed
		/// </summary>
		static std::vector<SceneCallback> m_SceneCallbacks;

		static SceneSystem* s_Instance;

	public:
		AquaAPI void Init() override;
		AquaAPI void Draw() override;
		AquaAPI void Update() override;
		AquaAPI void Destroy() override;

		/// <summary>
		/// Clears all current scenes & loads a new scene
		/// </summary>
		AquaAPI static void LoadScene(World* scene);

		/// <summary>
		/// Adds a scene, alongsdide already loaded scenes
		/// </summary>
		AquaAPI static void AddScene(World* scene);

		/// <summary>
		/// Removes a scene being active
		/// </summary>
		AquaAPI static void UnloadScene(int index);

		/// <summary>
		/// Removes a scene being active
		/// </summary>
		AquaAPI static void UnloadScene(World* scene);

		/// <summary>
		/// Unloads all scenes from being active
		/// </summary>
		AquaAPI static void UnloadAllScenes();

		/// <summary>
		/// Resets all active scenes by unloading and reloading them,
		/// in order they were added
		/// </summary>
		AquaAPI static void ReloadActiveScenes();
		
		/// <summary>
		/// Gets all actively loaded scenes
		/// </summary>
		AquaAPI static std::vector<World*>& GetActiveScenes();

		/// <summary>
		/// Adds a listener for when scenes are added & removed
		/// </summary>
		AquaAPI static void AddSceneCallback(SceneCallback callback);

		/// <summary>
		/// Removes a listener for when scenes are added & removed
		/// </summary>
		AquaAPI static void RemoveSceneCallback(SceneCallback callback);
	};
}