using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Camera : Component
	{
		/// <summary>
		/// Field of view
		/// </summary>
		public float FOV
		{
			get => _aqua_internal_Camera_GetFOV(World.ID, Entity.ID);
			set => _aqua_internal_Camera_SetFOV(World.ID, Entity.ID, value);
		}
		
		/// <summary>
		/// Near clipping plane
		/// </summary>
		public float Near
		{
			get => _aqua_internal_Camera_GetNear(World.ID, Entity.ID);
			set => _aqua_internal_Camera_SetNear(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Far clipping plane
		/// </summary>
		public float Far
		{
			get => _aqua_internal_Camera_GetFar(World.ID, Entity.ID);
			set => _aqua_internal_Camera_SetFar(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Is the camera in orthographic mode, or perspective?
		/// </summary>
		public bool Orthographic
		{
			get => _aqua_internal_Camera_GetOrtho(World.ID, Entity.ID);
			set => _aqua_internal_Camera_SetOrtho(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Size of the orthographic view
		/// </summary>
		public float OrthographicSize
		{
			get => _aqua_internal_Camera_GetOrthoSize(World.ID, Entity.ID);
			set => _aqua_internal_Camera_SetOrthoSize(World.ID, Entity.ID, value);
		}

		public static Camera Main
		{
			get
			{
				_aqua_internal_Camera_GetMainCamera(out uint worldID, out uint entityID);
				if (worldID == Entity.InvalidEntityID || entityID == Entity.InvalidEntityID)
					return null;
				return World.Get(worldID).GetEntity(entityID).GetComponent<Camera>();
			}
		}

		public void SetMain() => _aqua_internal_Camera_SetMainCamera(World.ID, Entity.ID);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_Camera_GetFOV(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Camera_SetFOV(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_Camera_GetNear(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Camera_SetNear(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_Camera_GetFar(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Camera_SetFar(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Camera_GetOrtho(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Camera_SetOrtho(uint worldID, uint entityID, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_Camera_GetOrthoSize(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Camera_SetOrthoSize(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Camera_SetMainCamera(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Camera_GetMainCamera(out uint worldID, out uint entityID);
		#endregion
	}
}
