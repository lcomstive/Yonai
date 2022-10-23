using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Transform : Component
	{
		public Vector3 Position
		{
			get
			{
				_aqua_internal_Transform_GetPosition(World.ID, Entity.ID, out Vector3 position);
				return position;
			}
			set => _aqua_internal_Transform_SetPosition(World.ID, Entity.ID, ref value);
		}

		public Quaternion Rotation
		{
			get
			{
				_aqua_internal_Transform_GetRotation(World.ID, Entity.ID, out Quaternion rotation);
				return rotation;
			}
			set => _aqua_internal_Transform_SetRotation(World.ID, Entity.ID, ref value);
		}

		public Vector3 Scale
		{
			get
			{
				_aqua_internal_Transform_GetScale(World.ID, Entity.ID, out Vector3 scale);
				return scale;
			}
			set => _aqua_internal_Transform_SetScale(World.ID, Entity.ID, ref value);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Transform_SetPosition(uint worldID, uint entityID, ref Vector3 position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Transform_GetPosition(uint worldID, uint entityID, out Vector3 position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Transform_SetRotation(uint worldID, uint entityID, ref Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Transform_GetRotation(uint worldID, uint entityID, out Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Transform_SetScale(uint worldID, uint entityID, ref Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Transform_GetScale(uint worldID, uint entityID, out Vector3 scale);
		#endregion
	}
}
