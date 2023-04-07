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
			Component[] components = GetComponents(true /* include inactive */);
			foreach (Component component in components)
				componentsArray.Add(component.OnSerialize());
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

				component.OnDeserialize(componentJSON);
				m_Components.Add(type, component);
			}
		}

		/// <summary>
		/// Removes an entity from <see cref="World"/>
		/// </summary>
		public void Destroy()
		{
			if (World.Exists(ID))
			{
				// Calls this function again, but updates the World as well
				World.DestroyEntity(ID);
				return;
			}

			foreach (var component in m_Components)
			{
				if (component.Value.Enabled)
					component.Value.Enabled = false; // Call OnDisabled
				component.Value._Destroy();
			}
		}

		/// <returns>True if this entity has an attached <see cref="Component"/></returns>
		public bool HasComponent<T>() where T : Component =>
			m_Components.ContainsKey(typeof(T)) || _HasComponent(World.ID, ID, typeof(T));

		/// <returns>New instance of <see cref="Component"/>, or existing instance if already on this entity</returns>
		public T AddComponent<T>(bool enable = true) where T : Component
		{
			Type type = typeof(T);
			if (HasComponent<T>())
				return (T)m_Components[type];

			T component = (T)_AddComponent(World.ID, ID, type, out IntPtr handle);
			if (component == null)
				return null;
			component.Handle = handle;
			m_Components.Add(type, component);

			if (World.IsActive)
			{
				component._Enable(enable, true);
				if(enable)
					component._Start();
			}
			else
				component.m_Enabled = enable; // Silently set enabled status

			return component;
		}

		/// <returns>Instance of <see cref="Component"/> attached to entity, or null if not found</returns>
		public T GetComponent<T>() where T : Component
		{
			if (!HasComponent<T>())
				return null;

			Type type = typeof(T);
			if (!m_Components.ContainsKey(type))
				m_Components.Add(type, (Component)_GetComponent(World.ID, ID, typeof(T)));
			return (T)m_Components[type];
		}

		public Component[] GetComponents(bool includeInactive = false)
		{
			/*
			List<Component> list = new List<Component>();
			object[] instances = _GetComponents(World.ID, ID, includeInactive);
			for (int i = 0; i < instances.Length; i++)
			{
				if (instances[i] == null)
					continue;

				Component component = instances[i] as Component;
				if (component)
					list.Add(component);
				else
					Log.Warning($"Instance not found for '{instances[i].GetType().Name}'");
			}

			return list.ToArray();
			*/
			return m_Components.Values.ToArray();
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

			return _RemoveComponent(World.ID, ID, type);
		}

		public override string ToString() => $"[{World.ID}:{ID}]";

		public static implicit operator string(Entity v) => v.ToString();

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _HasComponent(ulong worldID, ulong entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _GetComponent(ulong worldID, ulong entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object[] _GetComponents(ulong worldID, ulong entityID, bool includeInactive);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _AddComponent(ulong worldID, ulong entityID, Type type, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _RemoveComponent(ulong worldID, ulong entityID, Type type);
		#endregion
	}
}
