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
		std::vector<World*> m_ActiveScenes;

		/// <summary>
		/// Callback listeners for when scenes are added & removed
		/// </summary>
		std::vector<SceneCallback> m_SceneCallbacks;

	public:
		AquaAPI void Draw() override;
		AquaAPI void Update() override;

		/// <summary>
		/// Clears all current scenes & loads a new scene
		/// </summary>
		AquaAPI void LoadScene(World* scene);

		/// <summary>
		/// Adds a scene, alongsdide already loaded scenes
		/// </summary>
		AquaAPI void AddScene(World* scene);

		/// <summary>
		/// Removes a scene being active
		/// </summary>
		AquaAPI void UnloadScene(int index);

		/// <summary>
		/// Removes a scene being active
		/// </summary>
		AquaAPI void UnloadScene(World* scene);

		/// <summary>
		/// Unloads all scenes from being active
		/// </summary>
		AquaAPI void UnloadAllScenes();
		
		/// <summary>
		/// Gets all actively loaded scenes
		/// </summary>
		AquaAPI std::vector<World*>& GetActiveScenes();

		/// <summary>
		/// Adds a listener for when scenes are added & removed
		/// </summary>
		AquaAPI void AddSceneCallback(SceneCallback callback);

		/// <summary>
		/// Removes a listener for when scenes are added & removed
		/// </summary>
		AquaAPI void RemoveSceneCallback(SceneCallback callback);
	};
}