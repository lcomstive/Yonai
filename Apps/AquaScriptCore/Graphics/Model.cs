using System;
using AquaEngine.IO;
using System.Runtime.CompilerServices;
using Newtonsoft.Json.Linq;

namespace AquaEngine.Graphics
{
	public struct ModelImportSettings : IImportSettings
	{
		public string FilePath;

		public ModelImportSettings(string filePath) => FilePath = filePath;
	}

	public class Model : NativeResourceBase, ISerializable
	{
		public string FilePath { get; private set; }

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
			_Import(Handle, FilePath = settings.FilePath);

			// Get meshes
			_GetMeshes(Handle, out ulong[] meshIDs, out ulong[] materialIDs);

			Meshes = new MeshData[meshIDs.Length];
			for (int i = 0; i < meshIDs.Length; i++)
				// TODO: ERROR HERE. TRYING TO GET MATERIAL BUT MESH ID
				Meshes[i] = new MeshData(meshIDs[i], materialIDs[i]);

		}

		public JObject OnSerialize() => new JObject(
				new JProperty("FilePath", FilePath)
			);

		public void OnDeserialize(JObject json) => Import(new ModelImportSettings() { FilePath = json["FilePath"].Value<string>() });

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Import(IntPtr handle, string filepath);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetMeshes(IntPtr handle, out ulong[] meshIDs, out ulong[] materialIDs);
		#endregion
	}
}

