﻿using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;

namespace Yonai
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

				Component component = _AddComponent(World.ID, ID, type, out IntPtr handle) as Component;
				if(component == null)
				{
					Log.Warning($"Failed to deserialise type '{type.FullName}' as a Yonai.Component");
					continue;
				}
				component.SetHandle(handle);
				if (component == null)
				{
					Log.Warning($"Failed to add component of type '{fullName}' during deserialization");
					continue;
				}

				// component.OnDeserialize(componentJSON);
				m_Components.Add(type, component);
			}
		}

		/// <summary>
		/// Call each component's OnDeserialize.<br></br>
		/// This is done after initial serialization so references to components on other entities are valid
		/// </summary>
		internal void OnPostDeserialize(JObject json)
		{
			JArray componentsArray = json["Components"].Value<JArray>();
			foreach (JObject componentJSON in componentsArray)
			{
				Type type = Type.GetType(componentJSON["ComponentType"].Value<string>());
				if (type == null)
					continue;

				Component component = GetComponent(type);
				component.OnDeserialize(componentJSON);
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
			Type disposableType = typeof(IDisposable);
			foreach (var pair in m_Components)
			{
				if(disposableType.IsAssignableFrom(pair.Key))
					((IDisposable)pair.Value).Dispose();

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
		public T AddComponent<T>() where T : Component => (T)AddComponent(typeof(T));

		public Component AddComponent(Type componentType)
		{
			if (!componentType.IsSubclassOf(typeof(Component)))
				throw new Exception("AddComponent but componentType does not inherit from AquaEngine.Component");

			if (HasComponent(componentType))
				return m_Components[componentType];

			Component component = (Component)_AddComponent(World.ID, ID, componentType, out IntPtr handle);
			if (component == null)
				return null;
			component.SetHandle(handle);
			m_Components.Add(componentType, component);

			// Handle behaviour
			if (World.IsActive)
			{
				if(typeof(IBehaviour).IsAssignableFrom(componentType))
				{
					IBehaviour behaviour = (IBehaviour)component;
					behaviour.Enabled = true;
					behaviour.Start();
				}
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
		public bool RemoveComponent<T>() where T : Component => RemoveComponent(typeof(T));

		public bool RemoveComponent(Type type)
		{
			if (!type.IsSubclassOf(typeof(Component)))
				throw new Exception("Cannot remove component of type that does not inherit AquaEngine.Component");

			if (!m_Components.ContainsKey(type))
				return false; // Not on entity

			Component component = m_Components[type];
			m_Components.Remove(type);

			// Handle behaviour
			if (World.IsActive)
			{
				if (typeof(IBehaviour).IsAssignableFrom(type))
				{
					IBehaviour behaviour = (IBehaviour)component;
					behaviour.Enabled = false;
					behaviour.Destroyed();
				}
				if(typeof(IDisposable).IsAssignableFrom(type))
					((IDisposable)component).Dispose();
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