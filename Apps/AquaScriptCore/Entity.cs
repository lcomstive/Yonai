using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Entity
	{
		public uint ID { get; internal set; }
		public World World { get; internal set; }

		public const uint InvalidEntityID = uint.MaxValue;

		internal Entity(World world, uint id)
		{
			World = world;
			ID = id;
		}

		public bool HasComponent<T>() => _aqua_internal_Entity_HasComponent(World.ID, ID, typeof(T));

		/// <returns>New instance of component, or existing instance if already on this entity</returns>
		public T AddComponent<T>() where T : Component
		{

			return null;
		}

		public T GetComponent<T>() where T : Component => (T)_aqua_internal_Entity_GetComponent(World.ID, ID, typeof(T));

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Entity_HasComponent(uint worldID, uint entityID, Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _aqua_internal_Entity_GetComponent(uint worldID, uint entityID, Type type);
		#endregion
	}
}
