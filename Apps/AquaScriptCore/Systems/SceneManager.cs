using System.Collections.Generic;

namespace AquaEngine
{
	public class SceneManager : System
	{
		private static List<World> s_ActiveScenes = new List<World>();

		/// <summary>
		/// Clears all currently active scenes and loads a new one
		/// </summary>
		public static void Load(World world) { }

		/// <summary>
		/// Adds a new scene, alongside already loaded scenes
		/// </summary>
		public static void Add(World world) { }

		/// <summary>
		/// Removes a scene from being active
		/// </summary>
		public static void Unload(World world) { }

		/// <summary>
		/// Removes all scenes from being active
		/// </summary>
		public static void UnloadAll() { }

		/// <summary>
		/// Reloads all currently active scenes, by unloading & loading them in order they were added
		/// </summary>
		public static void Reload() { }

		public static List<World> GetActiveScenes() => s_ActiveScenes;

		public delegate void OnWorldChanged(World world, bool added);

		/// <summary>
		/// Called when a world is added or removed from the list of active scenes
		/// </summary>
		public static OnWorldChanged WorldChanged;
	}
}
