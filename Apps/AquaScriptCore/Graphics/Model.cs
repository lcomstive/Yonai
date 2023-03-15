using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public class Model : NativeResourceBase
	{
		public struct MeshData
		{
			public Mesh Mesh;
			public Material Material;

			public MeshData(uint meshID, uint materialID)
			{
				Mesh = Resource.Get<Mesh>(meshID);
				Material = Resource.Get<Material>(materialID);
			}
		}

		/// <summary>
		/// Meshes that make up this model
		/// </summary>
		public MeshData[] Meshes { get; private set; }

		internal override bool Load(string path, params object[] args)
		{
			uint resourceID;
			IntPtr handle;

			if(args.Length < 1 || !(args[0] is string))
					throw new ArgumentException("Model loading requires parameters of type string, representing virtual filesystem path (e.g. assets://cube.fbx)");

			_Load(path, (string)args[0], out resourceID, out handle);

			ResourceID = resourceID;
			Handle = handle;
			return true;
		}

		internal override void OnLoad()
		{
			_GetMeshes(Handle, out uint[] meshIDs, out uint[] materialIDs);

			Meshes = new MeshData[meshIDs.Length];
			for(int i = 0; i < meshIDs.Length; i++)
				Meshes[i] = new MeshData(meshIDs[i], materialIDs[i]);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, string filepath, out uint resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetMeshes(IntPtr handle, out uint[] meshIDs, out uint[] materialIDs);
		#endregion
	}
}

