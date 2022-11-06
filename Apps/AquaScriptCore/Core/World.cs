using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class World
	{
		public string Name { get; private set; }
		public uint ID { get; private set; }

		private Dictionary<uint, Entity> m_Entities = new Dictionary<uint, Entity>();

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

		#region Entities
		/// <returns>Instance of entity matching ID, or null if does not exist in this world</returns>
		public Entity GetEntity(uint entityID) => HasEntity(entityID) ? m_Entities[entityID] : null;

		/// <returns>True if entity with matching ID exists in this world</returns>
		public bool HasEntity(uint entityID)
		{
			bool hasEntity = _aqua_internal_World_HasEntity(ID, entityID);
			bool dictionaryHasEntity = m_Entities.ContainsKey(entityID);
			if (!hasEntity && dictionaryHasEntity)
				m_Entities.Remove(entityID);
			if (hasEntity && !dictionaryHasEntity)
				m_Entities.Add(entityID, new Entity(this, entityID));
			return hasEntity;
		}

		public void DestroyEntity(uint entityID)
		{
			_aqua_internal_World_DestroyEntity(ID, entityID);
			if (m_Entities.ContainsKey(entityID))
				m_Entities.Remove(entityID);
		}

		public List<Entity> GetEntities()
		{
			List<Entity> entities = new List<Entity>();
			foreach (var pair in m_Entities)
				entities.Add(pair.Value);
			return entities;
		}
		#endregion

		#region Systems
		public bool HasSystem<T>() => _aqua_internal_World_HasSystem(ID, typeof(T));

		public T AddSystem<T>() => (T)_aqua_internal_World_AddSystem(ID, typeof(T));

		public T GetSystem<T>() => (T)_aqua_internal_World_GetSystem(ID, typeof(T));

		public bool RemoveSystem<T>() => _aqua_internal_World_RemoveSystem(ID, typeof(T));
		#endregion

		#region Static getters
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
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_Destroy(uint id);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_World_Create(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_Exists(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_Get(uint worldID, out string name);

		// Entities
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_World_HasEntity(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_World_CreateEntity(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_World_DestroyEntity(uint worldID, uint entityID);

		// Systems
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _aqua_internal_World_HasSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _aqua_internal_World_GetSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _aqua_internal_World_AddSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _aqua_internal_World_RemoveSystem(uint worldID, Type type);
		#endregion
	}
}
