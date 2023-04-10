using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class MeshRenderer : Component
	{
		private UUID m_Mesh = uint.MaxValue;
		public UUID Mesh
		{
			get => m_Mesh;
			set
			{
				if(m_Mesh != value) // Check for change
					_SetMesh(Handle, (m_Mesh = value));
			}
		}

		private UUID m_Material = UUID.Invalid;
		public UUID Material
		{
			get => m_Material;
			set
			{
				if(m_Material != value) // Check for change
					_SetMaterial(Handle, (m_Material = value));
			}
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetMesh(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMesh(IntPtr handle, ulong mesh);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetMaterial(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMaterial(IntPtr handle, ulong material);
		#endregion
	}
}

