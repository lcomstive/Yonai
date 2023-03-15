using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public class Mesh : NativeResourceBase
	{
		public struct Vertex
		{
			public Vector3 Position;
			public Vector3 Normal;
			public Vector2 TexCoords;
		}

		private uint[] m_Indices;
		private Vertex[] m_Vertices;

		/// <summary>
		/// The vertex buffer for this mesh
		/// </summary>
		public Vertex[] Vertices
		{
			get => m_Vertices;
			set
			{
				if(m_Vertices == value)
					return; // No change

				m_Vertices = value;
				UpdateVertices();
			}
		}

		/// <summary>
		/// The index buffer for this mesh
		/// </summary>
		public uint[] Indices
		{
			get => m_Indices;
			set
			{
				if(m_Indices == value)
					return; // No change

				m_Indices = value;
				_SetIndices(Handle, m_Indices);
			}
		}

		internal override bool Load(string path, params object[] args)
		{
			uint resourceID;
			IntPtr handle;

			if(args.Length >= 2)
			{
				if(!(args[0] is Vertex[]) || !(args[1] is uint[]))
					throw new ArgumentException("Mesh loading requires parameters of type Vertex[], uint[]");

				Vertex[] vertices = (Vertex[])args[0];
				uint[] indices = (uint[])args[1];
				DrawMode drawMode = args.Length >= 3 ? (DrawMode)args[2] : DrawMode.Triangles;

				_Load1(path, vertices, indices, (byte)drawMode, out resourceID, out handle);
			}
			else
				_Load0(path, out resourceID, out handle);

			ResourceID = resourceID;
			Handle = handle;
			return true;
		}

		internal override void OnLoad()
		{
			_GetVertices(Handle, out Vector3[] positions, out Vector3[] normals, out Vector2[] texCoords);

			m_Vertices = new Vertex[positions.Length];
			for(int i = 0;i < positions.Length;i++)
				m_Vertices[i] = new Vertex()
				{
					Position = positions[i],
					Normal = normals[i],
					TexCoords = texCoords[i]
				};

			_GetIndices(Handle, out m_Indices);
		}

		/// <summary>
		/// Uploads the vertex buffer to unmanaged memory
		/// </summary>
		public void UpdateVertices()
		{
			Vector3[] positions = new Vector3[m_Vertices.Length];
			Vector3[] normals = new Vector3[m_Vertices.Length];
			Vector2[] texCoords = new Vector2[m_Vertices.Length];
			for(int i = 0;i < m_Vertices.Length;i++)
			{
				positions[i] = m_Vertices[i].Position;
				normals[i] = m_Vertices[i].Normal;
				texCoords[i] = m_Vertices[i].TexCoords;
			}
			_SetVertices(Handle, positions, normals, texCoords);
		}

		/// <summary>
		/// Uploads the index buffer to unmanaged memory
		/// </summary>
		public void UpdateIndices() => _SetIndices(Handle, m_Indices);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load0(string path, out uint resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load1(string path, Vertex[] vertices, uint[] indices, byte drawMode, out uint resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVertices(IntPtr handle, Vector3[] positions, Vector3[] normals, Vector2[] texCoords);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetVertices(IntPtr handle, out Vector3[] positions, out Vector3[] normals, out Vector2[] texCoords);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetIndices(IntPtr handle, uint[] indices);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetIndices(IntPtr handle, out uint[] indices);
		#endregion
	}
}

