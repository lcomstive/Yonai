using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class MeshRenderer : Component
	{
		private uint m_Mesh = uint.MaxValue;
		public uint Mesh
		{
			get => m_Mesh;
			set
			{
				if(m_Mesh != value) // Check for change
					_SetMesh(Handle, (m_Mesh = value));
			}
		}

		private uint m_Material = uint.MaxValue;
		public uint Material
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
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMesh(IntPtr handle, uint mesh);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetMaterial(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMaterial(IntPtr handle, uint mesh);
		#endregion
	}
}

