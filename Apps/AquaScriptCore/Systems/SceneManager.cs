using System.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public enum SceneAddType { Single, Additive }

	public class SceneManager
	{
		private static Dictionary<UUID, World> m_ActiveWorlds = new Dictionary<UUID, World>();

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
			world.SetActive(true);

			m_ActiveWorlds.Add(world.ID, world);
			WorldChanged?.Invoke(world, true);
		}

		/// <summary>
		/// Removes a scene from being active
		/// </summary>
		public static void Unload(World world)
		{
			_Unload(world.ID);
			world.SetActive(false);
			m_ActiveWorlds.Remove(world.ID);

			WorldChanged?.Invoke(world, false);
		}

		/// <summary>
		/// Removes all scenes from being active
		/// </summary>
		public static void UnloadAll()
		{
			World[] worlds = m_ActiveWorlds.Values.ToArray();
			foreach (World world in worlds)
				Unload(world);
			_UnloadAll();
		}

		public static World[] GetActiveScenes() => m_ActiveWorlds.Values.ToArray();

		/// <summary>
		/// Reloads all currently active scenes from disk
		/// </summary>
		public static void ReloadScenes()
		{
			foreach(var pair in m_ActiveWorlds)
			{
				_Unload(pair.Key);
				_Load(pair.Key);
			}
		}

		public delegate void OnWorldChanged(World world, bool added);

		/// <summary>
		/// Called when a world is added or removed from the list of active scenes
		/// </summary>
		public static event OnWorldChanged WorldChanged;

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(ulong world);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _LoadAdditive(ulong world);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unload(ulong world);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _UnloadAll();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong[] _GetActiveScenes();

		private static void _UpdateScenes()
		{
			// Cache active worlds from unmanaged code
			ulong[] worldIDs = _GetActiveScenes();
			for (int i = 0; i < worldIDs.Length; i++)
			{
				if (m_ActiveWorlds.ContainsKey(worldIDs[i]))
					continue; // Already added

				World world = World.Get(worldIDs[i]);
				world.SetActive(true);
				m_ActiveWorlds.Add(world.ID, world);
				WorldChanged?.Invoke(world, true);
			}

			Log.Trace($"Found {worldIDs.Length} active worlds during startup");
		}
		#endregion
	}
}
