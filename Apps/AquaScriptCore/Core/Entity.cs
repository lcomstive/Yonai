using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("[{World.ID}:{ID}]")]
	public class Entity
	{
		/// <summary>
		/// Identifier of this entity, unique in the <see cref="World"/>
		/// </summary>
		public uint ID { get; internal set; }

		/// <summary>
		/// <see cref="World"/> this entity resides in
		/// </summary>
		public World World { get; internal set; }

		/// <summary>
		/// An identifier to represent an invalid entity
		/// </summary>
		public const uint InvalidEntityID = uint.MaxValue;

		/// <summary>
		/// Checks if <see cref="ID"/> is valid
		/// </summary>
		public bool IsValid => ID != InvalidEntityID;

		private Dictionary<Type, object> m_Components = new Dictionary<Type, object>();

		internal Entity(World world, uint id)
		{
			World = world;
			ID = id;
		}

		/// <summary>
		/// Removes an entity from <see cref="World"/>
		/// </summary>
		public void Destroy() => World.DestroyEntity(ID);

		/// <returns>True if this entity has an attached <see cref="Component"/></returns>
		public bool HasComponent<T>() where T : Component => _HasComponent(World.ID, ID, typeof(T));

		/// <returns>New instance of <see cref="Component"/>, or existing instance if already on this entity</returns>
		public T AddComponent<T>() where T : Component
		{
			Type type = typeof(T);
			T component = (T)_AddComponent(World.ID, ID, type);
			if (component == null)
				return null;
			m_Components.Add(type, component);
			return component;
		}

		/// <returns>Instance of <see cref="Component"/> attached to entity, or null if not found</returns>
		public T GetComponent<T>() where T : Component
		{
			if (!HasComponent<T>())
				return null;

			Type type = typeof(T);
			if (!m_Components.ContainsKey(type))
				m_Components.Add(type, _GetComponent(World.ID, ID, typeof(T)));
			return (T)m_Components[type];
		}

		public Component[] GetComponents()
		{
			List<Component> list = new List<Component>();
			object[] instances = _GetComponents(World.ID, ID);
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
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool	  _HasComponent(uint worldID, uint entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _GetComponent(uint worldID, uint entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object[] _GetComponents(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _AddComponent(uint worldID, uint entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool   _RemoveComponent(uint worldID, uint entityID, Type type);
		#endregion
	}
}
