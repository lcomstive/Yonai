using System;
using Yonai.IO;
using System.Linq;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Yonai
{
	public class World : NativeResourceBase, ISerializable
	{
		public string Name { get; private set; }

		public UUID ID => ResourceID;

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

		public Entity[] Entities
		{
			get
			{
				ulong[] entityIDs = _GetEntities(ID);
				if (entityIDs == null)
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
		}

		public int EntityCount => _GetEntityCount(ID);

		private Dictionary<UUID, Entity> m_Entities = new Dictionary<UUID, Entity>();
		private Dictionary<Type, YonaiSystem> m_Systems = new Dictionary<Type, YonaiSystem>();

		protected override void OnLoad()
		{
			Name = new VFSFile(ResourcePath).FileNameWithoutExtension;

			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);

			Handle = handle;
			ResourceID = resourceID;

			Loaded?.Invoke();
		}

		protected override void OnUnload()
		{
			SceneManager.Unload(this);

			UUID[] keys = m_Entities.Keys.ToArray();
			foreach (UUID entity in keys)
				DestroyEntity(entity);
		}

		public JObject OnSerialize()
		{
			JObject json = new JObject();

			// Systems //
			JObject systemsJSON = new JObject();
			YonaiSystem[] systems = GetSystems();
			foreach (YonaiSystem system in systems)
			{
				Type type = system.GetType();
				systemsJSON.Add(new JProperty($"{type.FullName}, {type.Assembly.GetName().Name}", system.OnSerialize()));
			}
			json.Add("Systems", systemsJSON);

			// Entities //
			JArray entityArray = new JArray();
			Entity[] entities = Entities;
			foreach (Entity entity in entities)
				entityArray.Add(entity.OnSerialize());
			json.Add("Entities", entityArray);

			Serialized?.Invoke();
			return json;
		}

		public void OnDeserialize(JObject json)
		{
			// Destroy previous entities, will be recreating them
			Entity[] previousEntities = Entities ?? new Entity[0];
			foreach (Entity entity in previousEntities)
			{
				entity.Destroy();
				_DestroyEntity(ID, entity.ID);
			}
			m_Entities.Clear();

			try
			{
				// Create entities and their components
				JArray entities = json["Entities"].Value<JArray>();
				foreach (JObject entityJSON in entities)
				{
					ulong id = ulong.Parse(entityJSON["ID"].Value<string>());
					UUID entityID = new UUID(id);
					if (_CreateEntityID(ID, entityID) == UUID.Invalid)
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

				// Add all systems
				if (json.ContainsKey("Systems"))
				{
					Type YonaiSystemType = typeof(YonaiSystem);
					JObject systems = json["Systems"].Value<JObject>();
					foreach (JToken token in systems.Children())
					{
						JProperty systemJSON = token.Value<JProperty>();
						Log.Trace(systemJSON.Name);
						Type systemType = Type.GetType(systemJSON.Name);
						if (systemType != null && YonaiSystemType.IsAssignableFrom(systemType))
						{
							YonaiSystem system = AddSystem(systemType);
							system.Enable(m_IsActive);
							JObject value = systemJSON.Value.Value<JObject>();
							system.OnDeserialize(value);
						}
						else
							Log.Warning($"Could not find YonaiSystem type '{systemJSON.Name}'");
					}
				}
			}
			catch(Exception e) { Log.Exception(e); }

			// If enabled in SceneManager, enable and start all components & systems
			if (m_IsActive)
				SetActive(true);

			Deserialized?.Invoke();
		}

		internal void SetActive(bool active)
		{
			if(m_IsActive == active) return; // No change

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

			// Handle systems
			YonaiSystem[] systems = m_Systems.Values.ToArray();
			foreach(YonaiSystem system in systems)
				system.Enable(m_IsActive);

			StateChanged?.Invoke(m_IsActive);
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
			if(!m_Entities.TryGetValue(entityID, out Entity entity))
			{
				Log.Warning($"Failed to destroy entity [{entityID}] - Could not find entity in world [{ID}]");
				return;
			}

			if(entity.TryGetComponent(out Transform transform))
			{
				// Destroy all child entities before destroying this entity
				Transform[] children = transform.GetChildren();
				foreach (Transform child in children)
					DestroyEntity(child.Entity.ID);

				// Inform parent of removal
				transform.Parent?.RemoveChild(transform);
			}

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
		public bool HasSystem<T>() => HasSystem(typeof(T));
		public bool HasSystem(Type type) => m_Systems.ContainsKey(type);

		public T AddSystem<T>() where T : YonaiSystem => (T)AddSystem(typeof(T));
		public YonaiSystem AddSystem(Type type)
		{
			if (m_Systems.ContainsKey(type))
				return m_Systems[type];

			YonaiSystem instance = (YonaiSystem)_AddSystem(ID, type);
			m_Systems.Add(type, instance);
			return instance;
		}

		public T GetSystem<T>() where T : YonaiSystem => (T)GetSystem(typeof(T));
		public YonaiSystem GetSystem(Type type) => (YonaiSystem)_GetSystem(ID, type);

		public bool RemoveSystem<T>() where T : YonaiSystem => RemoveSystem(typeof(T));
		public bool RemoveSystem(Type type)
		{
			bool nativeResult = _RemoveSystem(ID, type);
			bool result = m_Systems.Remove(type);
			return nativeResult || result;
		}

		public void EnableSystem<T>(bool enable) => _EnableSystem(ID, typeof(T), enable);
		public void EnableSystem(Type type, bool enable) => _EnableSystem(ID, type, enable);

		/// <summary>
		/// Gets all systems attached to world
		/// </summary>
		public YonaiSystem[] GetSystems() => m_Systems.Values.ToArray();
		#endregion

		// public static implicit operator bool(World world) => world != null;

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetAll(out ulong[] worldIDs);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetName(ulong worldID, string name);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong[] _GetEntities(ulong worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetEntityCount(ulong worldID);
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
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object[] _GetSystems(ulong worldID);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _AddSystem(ulong worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _RemoveSystem(ulong worldID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool   _EnableSystem(ulong worldID, Type type, bool enable);
		#endregion

		public delegate void OnSerialized();
		public event OnSerialized Serialized;

		public delegate void OnDeserialized();
		public event OnDeserialized Deserialized;

		public delegate void OnStateChanged(bool active);
		public event OnStateChanged StateChanged;

		private delegate void OnLoaded();
		private event OnLoaded Loaded;
	}
}
