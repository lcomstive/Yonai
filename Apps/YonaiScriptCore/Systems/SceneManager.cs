using System.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Yonai
{
	public enum SceneAddType { Single, Additive }

	public class SceneManager
	{
		/// <summary>
		/// Checks if multiple worlds have been loaded, using <see cref="SceneAddType.Additive"/>
		/// </summary>
		public static bool MultipleScenesActive => m_ActiveWorlds.Count > 1;

		private static Dictionary<UUID, World> m_ActiveWorlds = new Dictionary<UUID, World>();

		/// <summary>
		/// Loads a world, either by clearing existing world(s) (<see cref="SceneAddType.Single"/>)
		/// or alongside others (<see cref="SceneAddType.Additive"/>).
		/// </summary>
		public static void Load(World world, SceneAddType addType = SceneAddType.Single, bool active = true)
		{
			if (m_ActiveWorlds.ContainsKey(world))
				// If loading existing world, assume a reload
				Unload(world);

			switch(addType)
			{
				default:
				case SceneAddType.Single:
						UnloadAll();
						_Load(world.ID);
						break;
				case SceneAddType.Additive:
						_LoadAdditive(world.ID);
						break;
			}
			world.SetActive(active);

			m_ActiveWorlds.Add(world.ID, world);
			WorldChanged?.Invoke(world, true);
		}

		/// <summary>
		/// Removes a scene from being active
		/// </summary>
		public static void Unload(World world)
		{
			if (!m_ActiveWorlds.ContainsKey(world.ID))
				return;

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

				World world = Resource.Get<World>(worldIDs[i]);
				world.SetActive(true);
				m_ActiveWorlds.Add(world.ID, world);
				WorldChanged?.Invoke(world, true);
			}

			Log.Trace($"Found {worldIDs.Length} active worlds during startup");
		}
		#endregion
	}
}
