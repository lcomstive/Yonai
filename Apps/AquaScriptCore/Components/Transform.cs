using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Transform : Component
	{
		public Vector3 Position
		{
			get
			{
				_GetPosition(Handle, out Vector3 position);
				return position;
			}
			set => _SetPosition(Handle, ref value);
		}

		public Quaternion Rotation
		{
			get
			{
				_GetRotation(Handle, out Quaternion rotation);
				return rotation;
			}
			set => _SetRotation(Handle, ref value);
		}

		public Vector3 Scale
		{
			get
			{
				_GetScale(Handle, out Vector3 scale);
				return scale;
			}
			set => _SetScale(Handle, ref value);
		}

		[Serialize(false), HideInInspector]
		public Vector3 Right
		{
			get
			{
				_GetRight(Handle, out Vector3 value);
				return value;
			}
		}

		[Serialize(false), HideInInspector]
		public Vector3 Up
		{
			get
			{
				_GetUp(Handle, out Vector3 value);
				return value;
			}
		}

		[Serialize(false), HideInInspector]
		public Vector3 Forward
		{
			get
			{
				_GetForward(Handle, out Vector3 value);
				return value;
			}
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetPosition(IntPtr handle, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetPosition(IntPtr handle, out Vector3 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetRotation(IntPtr handle, ref Quaternion value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRotation(IntPtr handle, out Quaternion value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetScale(IntPtr handle, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetScale(IntPtr handle, out Vector3 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetUp(IntPtr handle, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRight(IntPtr handle, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetForward(IntPtr handle, out Vector3 value);
		#endregion
	}
}
