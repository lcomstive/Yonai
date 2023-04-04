using System;
using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class World : ISerializable
	{
		public string Name { get; private set; }
		public uint ID { get; private set; }

		private Dictionary<uint, Entity> m_Entities = new Dictionary<uint, Entity>();

		private static Dictionary<uint, World> s_Instances = new Dictionary<uint, World>();

		internal World(uint id, string name)
		{
			ID = id;
			Name = name;

			if (!s_Instances.ContainsKey(ID))
				s_Instances.Add(ID, this);
		}

		/// <summary>
		/// Removes this world and releases all associated resources
		/// </summary>
		/// <returns>Success state of destruction. Only fails if already destroyed.</returns>
		public bool Destroy()
		{
			if (s_Instances.ContainsKey(ID))
				s_Instances.Remove(ID);
			return _Destroy(ID);
		}

		public JObject OnSerialize()
		{
			JObject json = new JObject();
			json["Name"] = Name;

			JArray entityArray = new JArray();
			Entity[] entities = GetEntities();
			foreach (Entity entity in entities)
				entityArray.Add(entity.OnSerialize());
			json.Add("Entities", entityArray);
			return json;
		}

		public void OnDeserialize(JObject json)
		{
			// Destroy previous entities, will be recreating them
			Entity[] previousEntities = GetEntities();
			foreach (Entity entity in previousEntities)
				_DestroyEntity(ID, entity.ID);
			m_Entities.Clear();

			Name = json["Name"].Value<string>();

			JArray entities = json["Entities"].Value<JArray>();
			foreach(JObject entityJSON in entities)
			{
				uint entityID = entityJSON["ID"].Value<uint>();
				if(_CreateEntityID(ID, entityID) == uint.MaxValue)
				{
					Log.Warning($"Failed to deserialize entity with ID [{entityID}]");
					continue;
				}

				Entity entity = new Entity(this, entityID);
				entity.OnDeserialize(entityJSON);
				m_Entities.Add(entityID, entity);
			}
		}

		#region Entities
		/// <returns>Instance of entity matching ID, or null if does not exist in this world</returns>
		public Entity GetEntity(uint entityID) => HasEntity(entityID) ? m_Entities[entityID] : null;

		/// <summary>
		/// Creates a new entity in this world
		/// </summary>
		public Entity CreateEntity()
		{
			uint entityID = _CreateEntity(ID);
			Entity e = new Entity(this, entityID);
			m_Entities.Add(entityID, e);
			return e;
		}

		/// <returns>True if entity with matching ID exists in this world</returns>
		public bool HasEntity(uint entityID)
		{
			bool hasEntity = _HasEntity(ID, entityID);
			bool dictionaryHasEntity = m_Entities.ContainsKey(entityID);
			if (!hasEntity && dictionaryHasEntity)
				m_Entities.Remove(entityID);
			if (hasEntity && !dictionaryHasEntity)
				m_Entities.Add(entityID, new Entity(this, entityID));
			return hasEntity;
		}

		public void DestroyEntity(uint entityID)
		{
			_DestroyEntity(ID, entityID);
			if (m_Entities.ContainsKey(entityID))
				m_Entities.Remove(entityID);
		}

		public Entity[] GetEntities()
		{
			uint[] entityIDs = _GetEntities(ID);
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
			uint[] entityIDs = _GetComponents(ID, typeof(T));
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
			uint[] entityIDs = _GetComponentsMultiple(ID, new Type[] { typeof(T1), typeof(T2) });
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
			uint[] entityIDs = _GetComponentsMultiple(ID, new Type[] { typeof(T1), typeof(T2), typeof(T3) });
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
		public bool HasSystem<T>() => _HasSystem(ID, typeof(T));

		public T AddSystem<T>() => (T)_AddSystem(ID, typeof(T));

		public T GetSystem<T>() => (T)_GetSystem(ID, typeof(T));

		public bool RemoveSystem<T>() => _RemoveSystem(ID, typeof(T));
		#endregion

		#region Static getters
		/// <summary>
		/// Gets <see cref="World"/> with <see cref="ID"/> equal to <paramref name="id"/>, or null if doesn't exist
		/// </summary>
		public static World Get(uint id)
		{
			if (s_Instances.ContainsKey(id))
				return s_Instances[id];
			if (!_Get(id, out string name))
				return null;
			return new World(id, name);
		}

		public static World[] GetAll()
		{
			// Get all world IDs
			_GetAll(out uint[] worldIDs);

			// Retrieve all worlds by ID
			World[] worlds = new World[worldIDs.Length];
			for (int i = 0; i < worldIDs.Length; i++)
				worlds[i] = Get(worldIDs[i]);

			return worlds;
		}

		/// <summary>
		/// Generates a new <see cref="World"/>
		/// </summary>
		/// <param name="name">Display name</param>
		/// <returns>Instance of generated world</returns>
		public static World Create(string name)
		{
			uint worldID = _Create(name);
			return new World(worldID, name);
		}

		public static World Create(JObject json)
		{
			string name = json["Name"].Value<string>();
			uint worldID = _Create(name);
			World world = new World(worldID, name);
			world.OnDeserialize(json);
			return world;
		}

		/// <returns>True if world exists with matching ID</returns>
		public static bool Exists(uint id) => _Exists(id);

		public static implicit operator bool(World world) => world != null;
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Destroy(uint id);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _Create(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Exists(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Get(uint worldID, out string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetAll(out uint[] worldIDs);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint[] _GetEntities(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint[] _GetComponents(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint[] _GetComponentsMultiple(uint worldID, Type[] types);

		// Entities
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _HasEntity(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _CreateEntity(uint worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _CreateEntityID(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroyEntity(uint worldID, uint entityID);

		// Systems
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _HasSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _GetSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _AddSystem(uint worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _RemoveSystem(uint worldID, Type type);
		#endregion
	}
}
