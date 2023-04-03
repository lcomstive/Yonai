using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public enum SceneAddType { Single, Additive }

	public class SceneManager : AquaSystem
	{
		/// <summary>
		/// Clears all currently active scenes and loads a new one
		/// </summary>
		public static void Load(World world, SceneAddType addType = SceneAddType.Single)
		{
			switch(addType)
			{
				default:
				case SceneAddType.Single: _Load(world.ID); break;
				case SceneAddType.Additive: _LoadAdditive(world.ID); break;
			}
		}

		/// <summary>
		/// Removes a scene from being active
		/// </summary>
		public static void Unload(World world) => _Unload(world.ID);

		/// <summary>
		/// Removes all scenes from being active
		/// </summary>
		public static void UnloadAll() => _UnloadAll();

		public static World[] GetActiveScenes()
		{
			List<World> scenes = new List<World>();
			uint[] worldIDs = _GetActiveScenes();
			for(int i = 0; i < worldIDs.Length; i++)
				scenes.Add(World.Get(worldIDs[i]));
			return scenes.ToArray();
		}

		public delegate void OnWorldChanged(World world, bool added);

		/// <summary>
		/// Called when a world is added or removed from the list of active scenes
		/// </summary>
		public static event OnWorldChanged WorldChanged;

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(uint world);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _LoadAdditive(uint world);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unload(uint world);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _UnloadAll();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint[] _GetActiveScenes();
		
		// Called from unmanaged code
		private static void _OnSceneChanged(uint worldID, bool added) =>
			WorldChanged?.Invoke(World.Get(worldID), added);
		#endregion
	}
}
