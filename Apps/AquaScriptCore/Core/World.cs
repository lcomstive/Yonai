using System;
using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;

namespace AquaEngine
{
	public class World : ISerializable
	{		
		public string Name { get; private set; }
		public UUID ID { get; private set; }

		private bool m_IsActive = false;
		/// <summary>
		/// True when world is loaded and active in <see cref="SceneManager"/>
		/// </summary>
		public bool IsActive
		{
			get => m_IsActive;
			set
			{
				if (m_IsActive != value)
					SetActive(value);
			}
		}

		private Dictionary<UUID, Entity> m_Entities = new Dictionary<UUID, Entity>();

		private static Dictionary<UUID, World> s_Instances = new Dictionary<UUID, World>();

		internal World(UUID id, string name)
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

			Serialized?.Invoke();
			return json;
		}

		public void OnDeserialize(JObject json)
		{
			// Destroy previous entities, will be recreating them
			Entity[] previousEntities = GetEntities();
			foreach (Entity entity in previousEntities)
			{
				entity.Destroy();
				_DestroyEntity(ID, entity.ID);
			}
			m_Entities.Clear();

			Name = json["Name"].Value<string>();

			// Create entities and their components
			JArray entities = json["Entities"].Value<JArray>();
			foreach(JObject entityJSON in entities)
			{
				ulong id = ulong.Parse(entityJSON["ID"].Value<string>());
				UUID entityID = new UUID(id);
				if(_CreateEntityID(ID, entityID) == UUID.Invalid)
				{
					Log.Warning($"Failed to deserialize entity with ID [{entityID}]");
					continue;
				}

				Entity entity = new Entity(this, entityID);
				entity.OnDeserialize(entityJSON);
				m_Entities.Add(entityID, entity);
			}

			// Deserialize components in entities.
			// This is done afterwards to keep references to components in other entities
			foreach (JObject entityJSON in entities)
			{
				UUID entityID = new UUID(ulong.Parse(entityJSON["ID"].Value<string>()));
				m_Entities[entityID].OnPostDeserialize(entityJSON);
			}

			// If enabled in SceneManager, enable and start all components
			if (m_IsActive)
				SetActive(true);

			Deserialized?.Invoke();
		}

		internal void SetActive(bool active)
		{
			m_IsActive = active;

			Entity[] entities = m_Entities.Values.ToArray();

			// Handle behaviours
			Type behaviourType = typeof(IBehaviour);
			foreach (Entity entity in entities)
			{
				foreach (Component component in entity.GetComponents())
				{
					if (!behaviourType.IsAssignableFrom(component.GetType()))
						continue;
					IBehaviour behaviour = (IBehaviour)component;

					if(m_IsActive)
						behaviour.Start();
					else
						behaviour.Destroyed();
				}
			}
		}

		public override bool Equals(object obj)
		{
			UUID id = UUID.Invalid;
			if (obj is World) id = ((World)obj).ID;
			else if (obj is UUID) id = (UUID)obj;
			else if (obj is ulong) id = new UUID((ulong)obj);

			return id != UUID.Invalid && id == ID;
		}

		public override int GetHashCode() => ID.GetHashCode();

		#region Entities
		/// <returns>Instance of entity matching ID, or null if does not exist in this world</returns>
		public Entity GetEntity(UUID entityID) => HasEntity(entityID) ? m_Entities[entityID] : null;

		/// <summary>
		/// Creates a new entity in this world
		/// </summary>
		public Entity CreateEntity()
		{
			UUID entityID = _CreateEntity(ID);
			Entity e = new Entity(this, entityID);
			m_Entities.Add(entityID, e);
			return e;
		}

		/// <returns>True if entity with matching ID exists in this world</returns>
		public bool HasEntity(UUID entityID)
		{
			bool hasEntity = _HasEntity(ID, entityID);
			bool dictionaryHasEntity = m_Entities.ContainsKey(entityID);
			if (!hasEntity && dictionaryHasEntity)
				m_Entities.Remove(entityID);
			if (hasEntity && !dictionaryHasEntity)
				m_Entities.Add(entityID, new Entity(this, entityID));
			return hasEntity;
		}

		public void DestroyEntity(UUID entityID)
		{
			Entity entity = m_Entities[entityID];
			RemoveEntity(entityID);
			entity?.Destroy();
		}

		/// <summary>
		/// Removes an entity without calling <see cref="Entity.Destroy"/>
		/// </summary>
		/// <param name="entityID"></param>
		internal void RemoveEntity(UUID entityID)
		{
			if (m_Entities.ContainsKey(entityID))
				m_Entities.Remove(entityID);	
				
			_DestroyEntity(ID, entityID);
		}

		public Entity[] GetEntities()
		{
			ulong[] entityIDs = _GetEntities(ID);
			if(entityIDs == null)
				return null;

			// Get all entities from IDs
			Entity[] entities = new Entity[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				UUID id = entityIDs[i];
				if (!m_Entities.ContainsKey(id))
					m_Entities.Add(id, new Entity(this, id));
				entities[i] = m_Entities[id];
			}

			return entities;
		}

		public Component[] GetComponents(Type type)
		{
			ulong[] entityIDs = _GetComponents(ID, type);
			if (entityIDs == null) return new Component[0];

			Component[] components = new Component[entityIDs.Length];
			for(int i = 0; i < entityIDs.Length; i++)
			{
				UUID entityID = entityIDs[i];
				if (!m_Entities.ContainsKey(entityID))
					m_Entities.Add(entityID, new Entity(this, entityID));
				components[i] = m_Entities[entityID].GetComponent(type);
			}

			return components;
		}

		public T[] GetComponents<T>() where T : Component
		{
			Type type = typeof(T);
			ulong[] entityIDs = _GetComponents(ID, type);
			if (entityIDs == null) return new T[0];

			T[] components = new T[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				UUID entityID = entityIDs[i];
				if (!m_Entities.ContainsKey(entityID))
					m_Entities.Add(entityID, new Entity(this, entityID));
				components[i] = (T)m_Entities[entityID].GetComponent(type);
			}

			return components;
		}
		public (T1[], T2[]) GetComponents<T1, T2>() where T1 : Component where T2 : Component
		{
			ulong[] entityIDs = _GetComponentsMultiple(ID, new Type[] { typeof(T1), typeof(T2) });
			if (entityIDs == null)
				return (new T1[0], new T2[0]);

			T1[] components1 = new T1[entityIDs.Length];
			T2[] components2 = new T2[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				UUID entityID = entityIDs[i];
				if (!m_Entities.ContainsKey(entityID))
					m_Entities.Add(entityID, new Entity(this, entityID));
				components1[i] = m_Entities[entityID].GetComponent<T1>();
				components2[i] = m_Entities[entityID].GetComponent<T2>();
			}
			return (components1, components2);
		}
		
		public (T1[], T2[], T3[]) GetComponents<T1, T2, T3>() where T1 : Component where T2 : Component where T3 : Component
		{
			ulong[] entityIDs = _GetComponentsMultiple(ID, new Type[] { typeof(T1), typeof(T2), typeof(T3) });
			if (entityIDs == null)
				return (new T1[0], new T2[0], new T3[0]);

			T1[] components1 = new T1[entityIDs.Length];
			T2[] components2 = new T2[entityIDs.Length];
			T3[] components3 = new T3[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				UUID entityID = entityIDs[i];
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

		public void EnableSystem<T>(bool enable) => _EnableSystem(ID, typeof(T), enable);
		#endregion

		#region Static getters
		/// <summary>
		/// Gets <see cref="World"/> with <see cref="ID"/> equal to <paramref name="id"/>, or null if doesn't exist
		/// </summary>
		public static World Get(UUID id)
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
			_GetAll(out ulong[] worldIDs);

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
			UUID worldID = _Create(name);
			return new World(worldID, name);
		}

		public static World Create(JObject json)
		{
			string name = json["Name"].Value<string>();
			UUID worldID = _Create(name);
			World world = new World(worldID, name);
			world.OnDeserialize(json);
			return world;
		}

		/// <returns>True if world exists with matching ID</returns>
		public static bool Exists(UUID id) => _Exists(id);

		public static implicit operator bool(World world) => world != null;
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Destroy(ulong id);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _Create(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Exists(ulong worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Get(ulong worldID, out string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetAll(out ulong[] worldIDs);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong[] _GetEntities(ulong worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong[] _GetComponents(ulong worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong[] _GetComponentsMultiple(ulong worldID, Type[] types);

		// Entities
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _HasEntity(ulong worldID, ulong entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _CreateEntity(ulong worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _CreateEntityID(ulong worldID, ulong entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _DestroyEntity(ulong worldID, ulong entityID);

		// Systems
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _HasSystem(ulong worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _GetSystem(ulong worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _AddSystem(ulong worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _RemoveSystem(ulong worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _EnableSystem(ulong worldID, Type type, bool enable);
		#endregion

		public delegate void OnSerialized();
		public event OnSerialized Serialized;

		public delegate void OnDeserialized();
		public event OnDeserialized Deserialized;
	}
}
