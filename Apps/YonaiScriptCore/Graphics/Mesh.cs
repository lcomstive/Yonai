using System;
using System.Runtime.CompilerServices;
using static Yonai.Graphics.Mesh;

namespace Yonai.Graphics
{
	public struct MeshImportSettings : IImportSettings
	{
		public uint[] Indices;
		public DrawMode DrawMode;
		public Mesh.Vertex[] Vertices;
	}

	[Serialize(false)]
	public class Mesh : NativeResourceBase
	{
		public struct Vertex
		{
			public Vector3 Position;
			public Vector3 Normal;
			public Vector2 TexCoords;

			public Vertex(Vector3 pos, Vector3 normal, Vector2 texCoords)
			{
				Position = pos;
				Normal = normal;
				TexCoords = texCoords;
			}

			public float[] ToArray()
			{
				float[] output = new float[3 + 3 + 2];
				output[0] = Position.x;
				output[1] = Position.y;
				output[2] = Position.z;

				output[3] = Normal.x;
				output[4] = Normal.y;
				output[5] = Normal.z;

				output[6] = TexCoords.x;
				output[7] = TexCoords.y;

				return output;
			}
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

		// Loaded from existing unmanaged resource
		protected override void OnNativeLoad()
		{
			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);
			ResourceID = resourceID;
			Handle = handle;

			// Get existing vertices
			_GetVertices(Handle, out Vector3[] positions, out Vector3[] normals, out Vector2[] texCoords);
			m_Settings.Vertices = new Vertex[positions.Length];
			for(int i = 0; i < positions.Length; i++)
			{
				m_Settings.Vertices[i] = new Vertex()
				{
					Position = positions[i],
					Normal = normals[i],
					TexCoords = texCoords[i]
				};
			}

			// Get existing indices
			_GetIndices(Handle, out m_Settings.Indices);
		}

		// Loaded new from managed code
		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);
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

		public static UUID QuadID => Quad.ResourceID;
		public static UUID CubeID => Cube.ResourceID;
		public static UUID SphereID => Sphere.ResourceID;

		public static Mesh Quad { get; private set; }
		public static Mesh Cube { get; private set; }
		public static Mesh Sphere { get; private set; }

		private static Mesh LoadPrimitive(string name)
		{
			Model model = Resource.Load<Model>($"assets://Models/{name}.fbx", new ModelImportSettings(false));
			if (!model || model.Meshes == null || model.Meshes.Length == 0)
			{
				Log.Warning($"Failed to load primitive '{name}'");
				return null;
			}
			Log.Debug($"Loading mesh from {name}");
			return Resource.Load<Mesh>($"Meshes/{name}", new MeshImportSettings()
			{
				DrawMode = DrawMode.Triangles,
				Vertices = model.Meshes[0].Mesh.Vertices,
				Indices = model.Meshes[0].Mesh.Indices
			});
		}

		internal static void LoadPrimitives()
		{
			Quad	= LoadPrimitive("Quad");
			Cube	= LoadPrimitive("Cube");
			Sphere	= LoadPrimitive("Sphere");
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Import(IntPtr handle, Vertex[] vertices, uint[] indices, byte drawMode);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVertices(IntPtr handle, Vector3[] positions, Vector3[] normals, Vector2[] texCoords);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetVertices(IntPtr handle, out Vector3[] positions, out Vector3[] normals, out Vector2[] texCoords);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetIndices(IntPtr handle, uint[] indices);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetIndices(IntPtr handle, out uint[] indices);
		#endregion
	}

	public static class VertexExtension
	{
		public static byte[] ToByteArray(this Vertex[] vertices)
		{
			int vertexSize = (3 + 3 + 2) * sizeof(float);
			byte[] output = new byte[vertexSize * vertices.Length];
			for(int i = 0;i < vertices.Length;i++)
				Buffer.BlockCopy(vertices[i].ToArray(), 0, output, i * vertexSize, vertexSize);
			return output;
		}
	}
}

