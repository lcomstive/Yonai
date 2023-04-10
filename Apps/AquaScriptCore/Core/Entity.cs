using System;
using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("[{World.ID}:{ID}]")]
	public class Entity : ISerializable
	{
		/// <summary>
		/// Identifier of this entity, unique in the <see cref="World"/>
		/// </summary>
		public UUID ID { get; internal set; }

		/// <summary>
		/// <see cref="World"/> this entity resides in
		/// </summary>
		public World World { get; internal set; }

		/// <summary>
		/// Checks if <see cref="ID"/> is valid
		/// </summary>
		public bool IsValid => ID != UUID.Invalid;

		private Dictionary<Type, Component> m_Components = new Dictionary<Type, Component>();

		internal Entity(World world, UUID id)
		{
			World = world;
			ID = id;
		}

		internal Entity(World world, JObject json)
		{
			World = world;
			OnDeserialize(json);
		}

		public JObject OnSerialize()
		{
			JObject json = new JObject();
			json["ID"] = ID.ToString();

			JArray componentsArray = new JArray();
			Component[] components = GetComponents();
			foreach (Component component in components)
			{
				try { componentsArray.Add(component.OnSerialize()); }
				catch (Exception e) { Log.Exception(e); }
			}
			json.Add("Components", componentsArray);

			return json;
		}

		public void OnDeserialize(JObject json)
		{
			JArray componentsArray = json["Components"].Value<JArray>();
			foreach (JObject componentJSON in componentsArray)
			{
				string fullName = componentJSON["ComponentType"].Value<string>();
				Type type = Type.GetType(fullName);

				if (type == null)
				{
					Log.Error($"Failed to get type '{fullName}'");
					continue;
				}

				Component component = (Component)_AddComponent(World.ID, ID, type, out IntPtr handle);
				component.Handle = handle;
				if (component == null)
				{
					Log.Warning($"Failed to add component of type '{fullName}' during deserialization");
					continue;
				}

				try
				{
					component.OnDeserialize(componentJSON);
					m_Components.Add(type, component);
				}
				catch(Exception e)
				{ Log.Exception(e); }
			}
		}

		/// <summary>
		/// Removes an entity from <see cref="World"/>
		/// </summary>
		public void Destroy()
		{
			if (World.HasEntity(ID))
				World.RemoveEntity(ID);

			// Handle behaviour
			Type behaviourType = typeof(IBehaviour);
			foreach (var pair in m_Components)
			{
				if (!behaviourType.IsAssignableFrom(pair.Key))
					continue;

				IBehaviour behaviour = (IBehaviour)pair.Value;
				behaviour.Enabled = false;
				behaviour.Destroyed();
			}
		}

		/// <returns>True if this entity has an attached <see cref="Component"/></returns>
		public bool HasComponent(Type type) =>
			m_Components.ContainsKey(type) || _HasComponent(World.ID, ID, type);

		/// <returns>True if this entity has an attached <see cref="Component"/></returns>
		public bool HasComponent<T>() where T : Component => HasComponent(typeof(T));

		/// <returns>New instance of <see cref="Component"/>, or existing instance if already on this entity</returns>
		public T AddComponent<T>() where T : Component
		{
			Type type = typeof(T);
			if (HasComponent<T>())
				return (T)m_Components[type];

			T component = (T)_AddComponent(World.ID, ID, type, out IntPtr handle);
			if (component == null)
				return null;
			component.Handle = handle;
			m_Components.Add(type, component);

			// Handle behaviour
			if (World.IsActive)
			{
				Type behaviourType = typeof(IBehaviour);
				foreach (var pair in m_Components)
					if (behaviourType.IsAssignableFrom(pair.Key))
						((IBehaviour)pair.Value).Start();
			}

			return component;
		}

		/// <returns>Instance of <see cref="Component"/> attached to entity, or null if not found</returns>
		public Component GetComponent(Type type)
		{
			if (!HasComponent(type))
				return null;

			if (!m_Components.ContainsKey(type))
				m_Components.Add(type, (Component)_GetComponent(World.ID, ID, type));
			return m_Components[type];
		}

		public T GetComponent<T>() where T : Component => (T)GetComponent(typeof(T));

		public Component[] GetComponents() => m_Components.Values.ToArray();

		/// <returns>True if component exists on entity, false otherwise</returns>
		public bool TryGetComponent(Type type, out Component component)
		{
			component = GetComponent(type);
			return component != null;
		}
		
		/// <returns>True if component exists on entity, false otherwise</returns>
		public bool TryGetComponent<T>(out T component) where T : Component
		{
			component = GetComponent<T>();
			return component != null;
		}

		/// <summary>
		/// Removes a <see cref="Component"/> from this entity
		/// </summary>
		/// <returns>Success state of removal</returns>
		public bool RemoveComponent<T>() where T : Component
		{
			Type type = typeof(T);

			if (m_Components.ContainsKey(type))
				m_Components.Remove(type);

			// Handle behaviour
			if (World.IsActive)
			{
				Type behaviourType = typeof(IBehaviour);
				foreach (var pair in m_Components)
					if (behaviourType.IsAssignableFrom(pair.Key))
						((IBehaviour)pair.Value).Destroyed();
			}

			return _RemoveComponent(World.ID, ID, type);
		}

		public override string ToString() => $"[{World.ID}:{ID}]";

		public static implicit operator string(Entity v) => v.ToString();
		public static implicit operator bool(Entity a) => a != null && a.ID != UUID.Invalid;

		public static bool operator ==(Entity a, object other) => a.Equals(other);
		public static bool operator !=(Entity a, object other) => !a.Equals(other);

		public static bool operator ==(Entity a, Entity b) => (a?.ID ?? UUID.Invalid) == (b?.ID ?? UUID.Invalid);
		public static bool operator !=(Entity a, Entity b) => (a?.ID ?? UUID.Invalid) != (b?.ID ?? UUID.Invalid);

		public override bool Equals(object obj)
		{
			if (obj == null) return false;

			UUID comparisonID = UUID.Invalid;
			if(obj is Entity) comparisonID = ((Entity)obj).ID;
			else if(obj is UUID) comparisonID = (UUID)obj;
			else if(obj is ulong) comparisonID = new UUID((ulong)obj);

			return comparisonID != UUID.Invalid && comparisonID == ID;
		}

		public override int GetHashCode() => ID.GetHashCode();

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _HasComponent(ulong worldID, ulong entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _GetComponent(ulong worldID, ulong entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object[] _GetComponents(ulong worldID, ulong entityID, bool includeInactive);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _AddComponent(ulong worldID, ulong entityID, Type type, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _RemoveComponent(ulong worldID, ulong entityID, Type type);
		#endregion
	}
}
