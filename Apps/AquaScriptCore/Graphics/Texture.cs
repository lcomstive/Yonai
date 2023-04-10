using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System;
using System.IO;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public struct TextureImportSettings : IImportSettings
	{
		public bool HDR;
		public string FilePath;

		public TextureImportSettings(string filePath, bool hdr = false)
		{
			FilePath = filePath;
			HDR = hdr;
		}
	}

	public class Texture : NativeResourceBase, ISerializable
	{
		public bool HDR => m_ImportSettings.HDR;
		public string FilePath => m_ImportSettings.FilePath;

		public IVector2 Resolution
		{
			get
			{
				_GetResolution(Handle, out IVector2 result);
				return result;
			}
		}

		private TextureImportSettings m_ImportSettings;

		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			IntPtr handle;

			_Load(ResourcePath, out resourceID, out handle);
			ResourceID = resourceID;
			Handle = handle;

			m_ImportSettings = new TextureImportSettings()
			{
				FilePath = _GetPath(Handle),
				HDR = _GetHDR(Handle)
			};
		}

		protected override void OnNativeLoad() => OnLoad();

		protected override void OnImported()
		{
			if (!TryGetImportSettings(out m_ImportSettings))
				Log.Warning("Texture.OnImport invalid import settings?");
			_Import(Handle, FilePath, HDR);
		}

		public void Bind(uint index = 0) => _Bind(Handle, index);

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("FilePath", FilePath),
				new JProperty("HDR", HDR)
			);

		public void OnDeserialize(JObject json) =>
			Import(new TextureImportSettings()
			{
				HDR		 = json["HDR"].Value<bool>(),
				FilePath = json["FilePath"].Value<string>()
			});
			

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Import(IntPtr handle, string filePath, bool hdr);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Bind(IntPtr handle, uint index);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPath(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetHDR(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetResolution(IntPtr handle, out IVector2 resolution);
		#endregion
	}
}