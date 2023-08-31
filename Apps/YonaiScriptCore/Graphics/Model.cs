using System;
using Yonai.IO;
using System.Runtime.CompilerServices;
using Newtonsoft.Json.Linq;
using System.Net.Configuration;

namespace Yonai.Graphics
{
	public struct ModelImportSettings : IImportSettings
	{
		public bool ImportMaterials;

		public ModelImportSettings(bool importMaterials = true) => ImportMaterials = importMaterials;
	}

	[SerializeFileOptions(SaveSeparateFile = true)]
	public class Model : NativeResourceBase, ISerializable
	{
		public bool SerializeAsCache => true;
		
		public bool ImportMaterials { get; private set; }

		public struct MeshData
		{
			public Mesh Mesh;
			public Material Material;

			public MeshData(UUID meshID, UUID materialID)
			{
				Mesh = Resource.Get<Mesh>(meshID);
				Material = Resource.Get<Material>(materialID);
			}
		}

		/// <summary>
		/// Meshes that make up this model
		/// </summary>
		public MeshData[] Meshes { get; private set; }

		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);

			ResourceID = resourceID;
			Handle = handle;
		}

		protected override void OnNativeLoad() => OnLoad();

		protected override void OnImported()
		{
			// Load model
			TryGetImportSettings(out ModelImportSettings settings);
			byte[] modelData = VFS.Read(ResourcePath);
			_Import(Handle, ResourcePath, modelData, ImportMaterials = settings.ImportMaterials);

			// Get meshes
			_GetMeshes(Handle, out ulong[] meshIDs, out ulong[] materialIDs);

			Meshes = new MeshData[meshIDs.Length];
			for (int i = 0; i < meshIDs.Length; i++)
				Meshes[i] = new MeshData(meshIDs[i], materialIDs[i]);

		}

		public JObject OnSerialize() => new JObject(
				new JProperty("ImportMaterials", ImportMaterials)
			);

		public void OnDeserialize(JObject json) => Import(new ModelImportSettings(json["ImportMaterials"].Value<bool>()));

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Import(IntPtr handle, string filepath, byte[] modelData, bool importMaterials);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetMeshes(IntPtr handle, out ulong[] meshIDs, out ulong[] materialIDs);
		#endregion
	}
}

