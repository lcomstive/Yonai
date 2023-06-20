using System;
using AquaEngine.IO;
using Newtonsoft.Json.Linq;

namespace AquaEngine
{
	public abstract class AquaSystem : ISerializable
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
		/// Called when this system is created (before <see cref="OnEnabled"/>)
		/// </summary>
		protected virtual void Start() { }

		/// <summary>
		/// Called once per frame
		/// </summary>
		protected virtual void Update() { }

		/// <summary>
		/// Called when script has been enabled.
		/// Can also be called when attached to an <see cref="Entity"/>,
		/// this function is called before <see cref="Start"/>.
		/// </summary>
		protected virtual void Enabled() { }

		/// <summary>
		/// Called when the script has been disabled.
		/// This function is called before <see cref="OnDestroyed"/>.
		/// </summary>
		protected virtual void Disabled() { }

		/// <summary>
		/// Called when the attached <see cref="Entity"/> is about to be destroyed (after <see cref="OnDisabled"/>)
		/// </summary>
		protected virtual void Destroyed() { }

		public virtual JObject OnSerialize() => new JObject();

		public virtual void OnDeserialize(JObject json) { }

		public void Enable(bool enable = true) =>
			World._EnableSystem(World?.ID ?? GlobalWorldID, GetType(), enable);

		#region Global Systems
		private static UUID GlobalWorldID = UUID.Invalid;

		/// <summary>
		/// Checks if a system exists globally
		/// </summary>
		public static bool Has<T>() => World._HasSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Checks if a system exists globally
		/// </summary>
		public static bool Has(Type type) => World._HasSystem(GlobalWorldID, type);

		/// <summary>
		/// Adds a global system
		/// </summary>
		public static T Add<T>() => (T)World._AddSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Adds a global system
		/// </summary>
		public static AquaSystem Add(Type type) => World._AddSystem(GlobalWorldID, type) as AquaSystem;

		/// <summary>
		/// Gets a global system
		/// </summary>
		public static T Get<T>() => (T)World._GetSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Gets a global system
		/// </summary>
		public static AquaSystem Get(Type type) => World._GetSystem(GlobalWorldID, type) as AquaSystem;

		/// <summary>
		/// Removes a global system
		/// </summary>
		public static bool Remove<T>() => World._RemoveSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Removes a global system
		/// </summary>
		public static bool Remove(Type type) => World._RemoveSystem(GlobalWorldID, type);
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

