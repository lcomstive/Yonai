using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public abstract class AquaSystem
	{
		/// <summary>
		/// World that created this system
		/// </summary>
		public World World { get; private set; } = null;

		/// <summary>
		/// Called once per frame after <see cref="Update"/>,
		/// when the screen is being drawn to
		/// </summary>
		protected virtual void Draw() { }

		/// <summary>
		/// Called when this script is created and attached to an <see cref="Entity"/> (after <see cref="OnEnabled"/>)
		/// </summary>
		protected virtual void Start() { }

		/// <summary>
		/// Called once per frame
		/// </summary>
		protected virtual void Update() { }

		/// <summary>
		/// Called when script has been enabled.
		/// Can also be called when attached to an <see cref="Entity"/>,
		/// this function is called before <see cref="Start"/> in this case
		/// </summary>
		protected virtual void Enabled() { }

		/// <summary>
		/// Called when the script has been disabled.
		/// Can also be called when attached <see cref="Entity"/> is being destroyed,
		/// this function is called before <see cref="OnDestroyed"/> in this case
		/// </summary>
		protected virtual void Disabled() { }

		/// <summary>
		/// Called when the attached <see cref="Entity"/> is about to be destroyed (after <see cref="OnDisabled"/>)
		/// </summary>
		protected virtual void Destroyed() { }

		#region Global Systems
		private static uint GlobalWorldID = uint.MaxValue;

		/// <summary>
		/// Checks if a system exists globally
		/// </summary>
		public static bool Has<T>() => World._HasSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Adds a global system
		/// </summary>
		public static T Add<T>() => (T)World._AddSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Gets a global system
		/// </summary>
		public static T Get<T>() => (T)World._GetSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Removes a global system
		/// </summary>
		public static bool Remove<T>() => World._RemoveSystem(GlobalWorldID, typeof(T));
		#endregion

		#region Unmanaged Calls
		// Called from C++
		internal void aqua_Initialise(uint worldID) => World = worldID != uint.MaxValue ? World.Get(worldID) : null;

		// Called from C++
		private void aqua_Enable(bool enable)
		{
			if (enable)
				Enabled();
			else
				Disabled();
		}
		#endregion
	}
}

