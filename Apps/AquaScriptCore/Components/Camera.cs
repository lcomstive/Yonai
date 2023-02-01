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
			get => _GetFOV(World.ID, Entity.ID);
			set => _SetFOV(World.ID, Entity.ID, value);
		}
		
		/// <summary>
		/// Near clipping plane
		/// </summary>
		public float Near
		{
			get => _GetNear(World.ID, Entity.ID);
			set => _SetNear(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Far clipping plane
		/// </summary>
		public float Far
		{
			get => _GetFar(World.ID, Entity.ID);
			set => _SetFar(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Is the camera in orthographic mode, or perspective?
		/// </summary>
		public bool Orthographic
		{
			get => _GetOrtho(World.ID, Entity.ID);
			set => _SetOrtho(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Size of the orthographic view
		/// </summary>
		public float OrthographicSize
		{
			get => _GetOrthoSize(World.ID, Entity.ID);
			set => _SetOrthoSize(World.ID, Entity.ID, value);
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

		public void SetMain() => _SetMainCamera(World.ID, Entity.ID);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetFOV(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetFOV(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetNear(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetNear(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetFar(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetFar(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetOrtho(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetOrtho(uint worldID, uint entityID, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetOrthoSize(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetOrthoSize(uint worldID, uint entityID, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMainCamera(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetMainCamera(out uint worldID, out uint entityID);
		#endregion
	}
}
