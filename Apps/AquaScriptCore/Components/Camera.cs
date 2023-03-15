using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Camera : NativeComponent
	{
		/// <summary>
		/// Field of view
		/// </summary>
		public float FOV
		{
			get => _GetFieldOfView(Handle);
			set => _SetFieldOfView(Handle, value);
		}
		
		/// <summary>
		/// Near clipping plane
		/// </summary>
		public float Near
		{
			get => _GetNear(Handle);
			set => _SetNear(Handle, value);
		}

		/// <summary>
		/// Far clipping plane
		/// </summary>
		public float Far
		{
			get => _GetFar(Handle);
			set => _SetFar(Handle, value);
		}

		/// <summary>
		/// Is the camera in orthographic mode, or perspective?
		/// </summary>
		public bool Orthographic
		{
			get => _GetOrthographic(Handle);
			set => _SetOrthographic(Handle, value);
		}

		/// <summary>
		/// Size of the orthographic view
		/// </summary>
		public float OrthographicSize
		{
			get => _GetOrthographicSize(Handle);
			set => _SetOrthographicSize(Handle, value);
		}

		public static Camera Main
		{
			get
			{
				_GetMainCamera(out uint worldID, out uint entityID);
				if (worldID == Entity.InvalidEntityID || entityID == Entity.InvalidEntityID)
					return null;
				return World.Get(worldID).GetEntity(entityID).GetComponent<Camera>();
			}
		}

		public void SetMain() => _SetMainCamera(Handle);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetFieldOfView(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetFieldOfView(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetNear(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetNear(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetFar(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetFar(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetOrthographic(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetOrthographic(IntPtr handle, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetOrthographicSize(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetOrthographicSize(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMainCamera(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetMainCamera(out uint worldID, out uint entityID);
		#endregion
	}
}
