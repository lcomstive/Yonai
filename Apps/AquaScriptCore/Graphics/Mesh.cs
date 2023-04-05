using System;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public class MeshImportSettings : IImportSettings
	{
		public uint[] Indices = null;
		public Mesh.Vertex[] Vertices = null;
		public DrawMode DrawMode = DrawMode.Triangles;
	}

	public class Mesh : NativeResourceBase
	{
		public struct Vertex
		{
			public Vector3 Position;
			public Vector3 Normal;
			public Vector2 TexCoords;
		}

		private MeshImportSettings m_Settings;

		/// <summary>
		/// The vertex buffer for this mesh
		/// </summary>
		public Vertex[] Vertices
		{
			get => m_Settings.Vertices;
			set
			{
				if(m_Settings.Vertices == value)
					return; // No change

				m_Settings.Vertices = value;
				UpdateVertices();
			}
		}

		/// <summary>
		/// The index buffer for this mesh
		/// </summary>
		public uint[] Indices
		{
			get => m_Settings.Indices;
			set
			{
				if(m_Settings.Indices == value)
					return; // No change

				m_Settings.Indices = value;
				_SetIndices(Handle, m_Settings.Indices);
			}
		}

		public DrawMode DrawMode => m_Settings.DrawMode;

		protected override void OnLoad()
		{
			_Load(ResourcePath, out ulong resourceID, out IntPtr handle);
			ResourceID = resourceID;
			Handle = handle;
		}

		protected override void OnImported()
		{
			if (!TryGetImportSettings(out m_Settings))
				// Set to default values if invalid import settings
				m_Settings = new MeshImportSettings();

			// Update vertices, indices and drawmode in unmanaged code
			_Import(Handle, Vertices, Indices, (byte)DrawMode);
		}

		/// <summary>
		/// Uploads the vertex buffer to unmanaged memory
		/// </summary>
		public void UpdateVertices()
		{
			Vector3[] positions = new Vector3[m_Settings.Vertices.Length];
			Vector3[] normals = new Vector3[m_Settings.Vertices.Length];
			Vector2[] texCoords = new Vector2[m_Settings.Vertices.Length];
			for(int i = 0;i < m_Settings.Vertices.Length;i++)
			{
				positions[i] = m_Settings.Vertices[i].Position;
				normals[i]   = m_Settings.Vertices[i].Normal;
				texCoords[i] = m_Settings.Vertices[i].TexCoords;
			}
			_SetVertices(Handle, positions, normals, texCoords);
		}

		/// <summary>
		/// Uploads the index buffer to unmanaged memory
		/// </summary>
		public void UpdateIndices() => _SetIndices(Handle, m_Settings.Indices);

		public static UUID QuadID => _GetQuadID();
		public static UUID CubeID => _GetCubeID();
		public static UUID SphereID => _GetSphereID();

		public static Mesh Quad => Resource.Get<Mesh>(QuadID);
		public static Mesh Cube => Resource.Get<Mesh>(CubeID);
		public static Mesh Sphere => Resource.Get<Mesh>(SphereID);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Import(IntPtr handle, Vertex[] vertices, uint[] indices, byte drawMode);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVertices(IntPtr handle, Vector3[] positions, Vector3[] normals, Vector2[] texCoords);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetVertices(IntPtr handle, out Vector3[] positions, out Vector3[] normals, out Vector2[] texCoords);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetIndices(IntPtr handle, uint[] indices);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetIndices(IntPtr handle, out uint[] indices);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetQuadID();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetCubeID();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetSphereID();
		#endregion
	}
}

