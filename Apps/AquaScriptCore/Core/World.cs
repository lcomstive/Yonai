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
			return _aqua_internal_Destroy(ID);
		}

		#region Entities
		/// <returns>Instance of entity matching ID, or null if does not exist in this world</returns>
		public Entity GetEntity(uint entityID) => HasEntity(entityID) ? m_Entities[entityID] : null;

		/// <returns>True if entity with matching ID exists in this world</returns>
		public bool HasEntity(uint entityID)
		{
			bool hasEntity = _aqua_internal_HasEntity(ID, entityID);
			bool dictionaryHasEntity = m_Entities.ContainsKey(entityID);
			if (!hasEntity && dictionaryHasEntity)
				m_Entities.Remove(entityID);
			if (hasEntity && !dictionaryHasEntity)
				m_Entities.Add(entityID, new Entity(this, entityID));
			return hasEntity;
		}

		public void DestroyEntity(uint entityID)
		{
			_aqua_internal_DestroyEntity(ID, entityID);
			if (m_Entities.ContainsKey(entityID))
				m_Entities.Remove(entityID);
		}

		public Entity[] GetEntities()
		{
			uint[] entityIDs = _aqua_internal_GetEntities(ID);
			if(entityIDs == null)
				return null;

			// Get all entities from IDs
			Entity[] entities = new Entity[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				uint id = entityIDs[i];
				if (!m_Entities.ContainsKey(id))
					m_Entities.Add(id, new Entity(this, id));
				entities[i] = m_Entities[id];
			}

			return entities;
		}

		public T[] GetComponents<T>() where T : Component
		{
			uint[] entityIDs = _aqua_internal_GetComponents(ID, typeof(T));
			if (entityIDs == null)
				return new T[0];

			T[] components = new T[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				uint entityID = entityIDs[i];
				if (!m_Entities.ContainsKey(entityID))
					m_Entities.Add(entityID, new Entity(this, entityID));
				components[i] = m_Entities[entityID].GetComponent<T>();
			}
			return components;
		}
		
		public (T1[], T2[]) GetComponents<T1, T2>() where T1 : Component where T2 : Component
		{
			uint[] entityIDs = _aqua_internal_GetComponentsMultiple(ID, new Type[] { typeof(T1), typeof(T2) });
			if (entityIDs == null)
				return (new T1[0], new T2[0]);

			T1[] components1 = new T1[entityIDs.Length];
			T2[] components2 = new T2[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				uint entityID = entityIDs[i];
				if (!m_Entities.ContainsKey(entityID))
					m_Entities.Add(entityID, new Entity(this, entityID));
				components1[i] = m_Entities[entityID].GetComponent<T1>();
				components2[i] = m_Entities[entityID].GetComponent<T2>();
			}
			return (components1, components2);
		}
		
		public (T1[], T2[], T3[]) GetComponents<T1, T2, T3>() where T1 : Component where T2 : Component where T3 : Component
		{
			uint[] entityIDs = _aqua_internal_GetComponentsMultiple(ID, new Type[] { typeof(T1), typeof(T2), typeof(T3) });
			if (entityIDs == null)
				return (new T1[0], new T2[0], new T3[0]);

			T1[] components1 = new T1[entityIDs.Length];
			T2[] components2 = new T2[entityIDs.Length];
			T3[] components3 = new T3[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				uint entityID = entityIDs[i];
				if (!m_Entities.ContainsKey(entityID))
					m_Entities.Add(entityID, new Entity(this, entityID));
				components1[i] = m_Entities[entityID].GetComponent<T1>();
				components2[i] = m_Entities[entityID].GetComponent<T2>();
				components3[i] = m_Entities[entityID].GetComponent<T3>();
			}
			return (components1, components2, components3);
		}
		#endregion

		#region Systems
		public bool HasSystem<T>() => _aqua_internal_HasSystem(ID, typeof(T));

		public T AddSystem<T>() => (T)_aqua_internal_AddSystem(ID, typeof(T));

		public T GetSystem<T>() => (T)_aqua_internal_GetSystem(ID, typeof(T));

		public bool RemoveSystem<T>() => _aqua_internal_RemoveSystem(ID, typeof(T));
		#endregion

		#region Static getters
		/// <summary>
		/// Gets <see cref="World"/> with <see cref="ID"/> equal to <paramref name="id"/>, or null if doesn't exist
		/// </summary>
		public static World Get(uint id)
		{
			if (s_Instances.ContainsKey(id))
				return s_Instances[id];
			if (!_aqua_internal_Get(id, out string name))
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
			uint worldID = _aqua_internal_Create(name);
			return new World(worldID, name);
		}

		/// <returns>True if world exists with matching ID</returns>
		public static bool Exists(uint id) => _aqua_internal_Exists(id);
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Destroy(uint id);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_Create(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Exists(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Get(uint worldID, out string name);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint[] _aqua_internal_GetEntities(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint[] _aqua_internal_GetComponents(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint[] _aqua_internal_GetComponentsMultiple(uint worldID, Type[] types);

		// Entities
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_HasEntity(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_CreateEntity(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_DestroyEntity(uint worldID, uint entityID);

		// Systems
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _aqua_internal_HasSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _aqua_internal_GetSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _aqua_internal_AddSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _aqua_internal_RemoveSystem(uint worldID, Type type);
		#endregion
	}
}
