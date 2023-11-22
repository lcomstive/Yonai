using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics
{
	public struct ModelImportSettings : IImportSettings
	{
		public bool ImportMaterials;

		public ModelImportSettings(bool importMaterials = true) => ImportMaterials = importMaterials;
	}

	internal struct MeshImportData
	{
		public Mesh.Vertex[] Vertices;
		public uint[] Indices;
	}

	[SerializeFileOptions(SaveSeparateFile = true)]
	public class Model : ResourceBase, ISerializable
	{
		public bool SerializeAsCache => true;
		
		public bool ImportMaterials { get; private set; }

		/// <summary>
		/// Meshes that make up this model
		/// </summary>
		public Mesh[] Meshes { get; private set; }

		protected override void OnImported()
		{
			// Load model
			TryGetImportSettings(out ModelImportSettings settings);
			byte[] modelData = VFS.Read(ResourcePath);
			bool success = _Import(modelData, ImportMaterials = settings.ImportMaterials, out MeshImportData[] meshData);

			if(!success)
			{
				Log.Error("Failed to import model");
				Application.Exit();
				return;
			}

			Meshes = new Mesh[meshData.Length];
			for (int i = 0; i < meshData.Length; i++)
				Meshes[i] = Resource.Load<Mesh>($"{ResourcePath}/Mesh_{i}", new MeshImportSettings()
				{
					Vertices = meshData[i].Vertices,
					Indices = meshData[i].Indices
				});
		}

		public JObject OnSerialize() => new JObject(
				new JProperty("ImportMaterials", ImportMaterials)
			);

		public void OnDeserialize(JObject json) => Import(new ModelImportSettings(json["ImportMaterials"].Value<bool>()));

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _Import(byte[] modelData, bool importMaterials, out MeshImportData[] meshData);
		#endregion
	}
}

