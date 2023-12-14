using System;
using Yonai.Systems;

namespace Yonai.Graphics
{
	public struct MeshImportSettings : IImportSettings
	{
		public uint[] Indices;
		public Mesh.Vertex[] Vertices;
	}

	[Serialize(false)]
	public class Mesh : ResourceBase
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
				if (m_Settings.Vertices == value)
					return; // No change

				m_Settings.Vertices = value;
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
				if (m_Settings.Indices == value)
					return; // No change
				m_Settings.Indices = value;
			}
		}

		public IBuffer VertexBuffer { get; private set; }
		public IBuffer IndexBuffer { get; private set; }

		protected override void OnLoad() => GenerateBuffers();

		protected override void OnUnload()
		{
			VertexBuffer?.Dispose();
			IndexBuffer?.Dispose();
		}

		protected override void OnImported()
		{
			if (!TryGetImportSettings(out m_Settings))
				// Set to default values if invalid import settings
				m_Settings = new MeshImportSettings();
		}

		private void GenerateBuffers()
		{
			VertexBuffer?.Dispose();
			IndexBuffer?.Dispose();

			VertexBuffer = null;
			IndexBuffer = null;

			if (Vertices == null)
				return;

			IGraphicsDevice device = RenderSystem.Backend.Device;

			#region Vertex Buffer
			int bufferSize = sizeof(float) * 8 * Vertices.Length;

			// Upload data from CPU to GPU memory
			IBuffer stagingBuffer = device.CreateBuffer(bufferSize, BufferUsage.CPUWrite);
			stagingBuffer.Upload(Vertices.ToByteArray());

			// Copy staging buffer data into vertex buffer, which is GPU-only (so cannot upload directly from CPU)
			VertexBuffer = device.CreateBuffer(bufferSize, BufferUsage.NoCPU, BufferType.Vertex);
			device.CopyBuffer(stagingBuffer, VertexBuffer);

			stagingBuffer.Dispose();
			#endregion

			#region Index Buffer
			if (Indices != null)
			{
				// Upload data from CPU to GPU memory
				bufferSize = sizeof(uint) * Indices.Length;
				byte[] indicesData = new byte[bufferSize];
				Buffer.BlockCopy(Indices, 0, indicesData, 0, bufferSize);

				stagingBuffer = device.CreateBuffer(bufferSize, BufferUsage.CPUWrite);
				stagingBuffer.Upload(indicesData);

				// Copy staging buffer data into index buffer, which is GPU-only (so cannot upload directly from CPU)
				IndexBuffer = device.CreateBuffer(bufferSize, BufferUsage.NoCPU, BufferType.Index);
				device.CopyBuffer(stagingBuffer, IndexBuffer);

				stagingBuffer.Dispose();
			}
			#endregion
		}

		public static UUID QuadID => Quad.ResourceID;
		public static UUID CubeID => Cube.ResourceID;
		public static UUID SphereID => Sphere.ResourceID;

		public static Mesh Quad { get; private set; }
		public static Mesh Cube { get; private set; }
		public static Mesh Sphere { get; private set; }

		private static Mesh LoadPrimitive(string name)
		{
			Log.Debug($"Loading primitive {name}");
            Model model = Resource.Load<Model>($"assets://Models/{name}.fbx", new ModelImportSettings(false));
			if (!model || model.Meshes == null || model.Meshes.Length == 0)
			{
				Log.Warning($"Failed to load primitive '{name}'");
				return null;
			}
			return model.Meshes[0];
		}

		internal static void LoadPrimitives()
		{
			Quad = LoadPrimitive("Quad");
			Cube = LoadPrimitive("Cube");
			Sphere = LoadPrimitive("Sphere");
		}
	}

	public static class VertexExtension
	{
		public static byte[] ToByteArray(this Mesh.Vertex[] vertices)
		{
			int vertexSize = (3 + 3 + 2) * sizeof(float);
			byte[] output = new byte[vertexSize * vertices.Length];
			for(int i = 0;i < vertices.Length;i++)
				Buffer.BlockCopy(vertices[i].ToArray(), 0, output, i * vertexSize, vertexSize);
			return output;
		}
	}
}

