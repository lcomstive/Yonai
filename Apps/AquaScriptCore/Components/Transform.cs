using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Transform : Component
	{
		public Vector3 Position
		{
			get
			{
				_Transform_GetPosition(World.ID, Entity.ID, out Vector3 position);
				return position;
			}
			set => _Transform_SetPosition(World.ID, Entity.ID, ref value);
		}

		public Quaternion Rotation
		{
			get
			{
				_Transform_GetRotation(World.ID, Entity.ID, out Quaternion rotation);
				return rotation;
			}
			set => _Transform_SetRotation(World.ID, Entity.ID, ref value);
		}

		public Vector3 Scale
		{
			get
			{
				_Transform_GetScale(World.ID, Entity.ID, out Vector3 scale);
				return scale;
			}
			set => _Transform_SetScale(World.ID, Entity.ID, ref value);
		}

		public Vector3 Right
		{
			get
			{
				_Transform_GetRight(World.ID, Entity.ID, out Vector3 value);
				return value;
			}
		}

		public Vector3 Up
		{
			get
			{
				_Transform_GetUp(World.ID, Entity.ID, out Vector3 value);
				return value;
			}
		}

		public Vector3 Forward
		{
			get
			{
				_Transform_GetForward(World.ID, Entity.ID, out Vector3 value);
				return value;
			}
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_SetPosition(uint worldID, uint entityID, ref Vector3 position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_GetPosition(uint worldID, uint entityID, out Vector3 position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_SetRotation(uint worldID, uint entityID, ref Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_GetRotation(uint worldID, uint entityID, out Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_SetScale(uint worldID, uint entityID, ref Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_GetScale(uint worldID, uint entityID, out Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_GetUp(uint worldID, uint entityID, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_GetForward(uint worldID, uint entityID, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Transform_GetRight(uint worldID, uint entityID, out Vector3 scale);
		#endregion
	}
}
