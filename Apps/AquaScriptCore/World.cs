using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class World
	{
		public string Name { get; private set; }
		public uint ID { get; private set; }

		private static Dictionary<uint, World> s_Instances = new Dictionary<uint, World>();

		/// <summary>
		/// Currently loaded world.
		/// Set to first created instance by default, or null if none have been created
		/// </summary>
		public static World Current { get; private set; } = null;

		internal World(uint id, string name)
		{
			ID = id;
			Name = name;

			if (Current == null)
				Current = this;

			if (!s_Instances.ContainsKey(ID))
				s_Instances.Add(ID, this);
		}

		/// <summary>
		/// Removes this world and releases all associated resources
		/// </summary>
		/// <returns>Success state of destruction. Only fails if already destroyed.</returns>
		public bool Destroy()
		{
			if (Current == this)
				Current = null;
			if (s_Instances.ContainsKey(ID))
				s_Instances.Remove(ID);
			return _aqua_internal_World_Destroy(ID);
		}

		/// <returns>Instance of entity matching ID, or null if does not exist in this world</returns>
		public Entity GetEntity(uint entityID) => HasEntity(entityID) ? new Entity(this, entityID) : null;

		/// <returns>True if entity with matching ID exists in this world</returns>
		public bool HasEntity(uint entityID) => _aqua_internal_World_HasEntity(ID, entityID);

		/// <summary>
		/// Gets <see cref="World"/> with <see cref="ID"/> equal to <paramref name="id"/>, or null if doesn't exist
		/// </summary>
		public static World Get(uint id)
		{
			if (s_Instances.ContainsKey(id))
				return s_Instances[id];
			if (!_aqua_internal_World_Get(id, out string name))
				return null;
			return new World(id, name);
		}

		/// <summary>
		/// Generates a new <see cref="World"/>
		/// </summary>
		/// <param name="name">Display name</param>
		/// <returns>Instance of generated world</returns>
		public static World Create(string name)
		{
			uint worldID = _aqua_internal_World_Create(name);
			return new World(worldID, name);
		}

		/// <returns>True if world exists with matching ID</returns>
		public static bool Exists(uint id) => _aqua_internal_World_Exists(id);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_Destroy(uint id);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_World_Create(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_Exists(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_Get(uint worldID, out string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_HasEntity(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_World_CreateEntity(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_World_DestroyEntity(uint worldID, uint entityID);
		#endregion
	}
}
