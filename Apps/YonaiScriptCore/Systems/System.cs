using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;

namespace Yonai
{
	public abstract class YonaiSystem : ISerializable
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
		protected virtual void Start() { Log.Trace("Start for " + GetType().Name); }

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
		public static YonaiSystem Add(Type type) => World._AddSystem(GlobalWorldID, type) as YonaiSystem;

		/// <summary>
		/// Gets a global system
		/// </summary>
		public static T Get<T>() => (T)World._GetSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Gets a global system
		/// </summary>
		public static YonaiSystem Get(Type type) => World._GetSystem(GlobalWorldID, type) as YonaiSystem;

		/// <summary>
		/// Gets all global systems
		/// </summary>
		public static YonaiSystem[] GetAll()
		{
			object[] objs = World._GetSystems(GlobalWorldID);
			YonaiSystem[] systems = new YonaiSystem[objs.Length];
			for(int i = 0; i < objs.Length; i++)
				systems[i] = objs[i] as YonaiSystem;
			return systems;
		}

		/// <summary>
		/// Removes a global system
		/// </summary>
		public static bool Remove<T>() => World._RemoveSystem(GlobalWorldID, typeof(T));

		/// <summary>
		/// Removes a global system
		/// </summary>
		public static bool Remove(Type type) => World._RemoveSystem(GlobalWorldID, type);
		#endregion

		public static implicit operator bool(YonaiSystem system) => system != null;

		#region Unmanaged Calls
		// Called from C++
		internal void _Initialise(ulong worldID) => World = worldID != UUID.Invalid ? World.Get(worldID) : null;

		// Called from C++
		private void _Enable(bool enable)
		{
			if (enable)
				Enabled();
			else
				Disabled();
		}
		#endregion
	}
}

