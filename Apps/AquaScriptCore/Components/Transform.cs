using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Transform : Component
	{
		public Vector3 Position
		{
			get
			{
				_GetPosition(World.ID, Entity.ID, out Vector3 position);
				return position;
			}
			set => _SetPosition(World.ID, Entity.ID, ref value);
		}

		public Quaternion Rotation
		{
			get
			{
				_GetRotation(World.ID, Entity.ID, out Quaternion rotation);
				return rotation;
			}
			set => _SetRotation(World.ID, Entity.ID, ref value);
		}

		public Vector3 Scale
		{
			get
			{
				_GetScale(World.ID, Entity.ID, out Vector3 scale);
				return scale;
			}
			set => _SetScale(World.ID, Entity.ID, ref value);
		}

		public Vector3 Right
		{
			get
			{
				_GetRight(World.ID, Entity.ID, out Vector3 value);
				return value;
			}
		}

		public Vector3 Up
		{
			get
			{
				_GetUp(World.ID, Entity.ID, out Vector3 value);
				return value;
			}
		}

		public Vector3 Forward
		{
			get
			{
				_GetForward(World.ID, Entity.ID, out Vector3 value);
				return value;
			}
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetPosition(uint worldID, uint entityID, ref Vector3 position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetPosition(uint worldID, uint entityID, out Vector3 position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetRotation(uint worldID, uint entityID, ref Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRotation(uint worldID, uint entityID, out Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetScale(uint worldID, uint entityID, ref Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetScale(uint worldID, uint entityID, out Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetUp(uint worldID, uint entityID, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetForward(uint worldID, uint entityID, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRight(uint worldID, uint entityID, out Vector3 scale);
		#endregion
	}
}
