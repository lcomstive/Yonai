using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
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

		internal Entity(World world, uint id)
		{
			World = world;
			ID = id;
		}

		/// <returns>True if this entity has an attached <see cref="Component"/></returns>
		public bool HasComponent<T>() where T : Component => _aqua_internal_Entity_HasComponent(World.ID, ID, typeof(T));

		/// <returns>New instance of <see cref="Component"/>, or existing instance if already on this entity</returns>
		public T AddComponent<T>() where T : Component => (T)_aqua_internal_Entity_AddComponent(World.ID, ID, typeof(T));

		/// <returns>Instance of <see cref="Component"/> attached to entity, or null if not found</returns>
		public T GetComponent<T>() where T : Component => (T)_aqua_internal_Entity_GetComponent(World.ID, ID, typeof(T));

		/// <summary>
		/// Removes a <see cref="Component"/> from this entity
		/// </summary>
		/// <returns>Success state of removal</returns>
		public bool RemoveComponent<T>() where T : Component => _aqua_internal_Entity_RemoveComponent(World.ID, ID, typeof(T));

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Entity_HasComponent(uint worldID, uint entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _aqua_internal_Entity_GetComponent(uint worldID, uint entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _aqua_internal_Entity_AddComponent(uint worldID, uint entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Entity_RemoveComponent(uint worldID, uint entityID, Type type);
		#endregion
	}
}
