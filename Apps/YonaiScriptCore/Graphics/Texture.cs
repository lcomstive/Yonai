using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics
{
	public enum TextureFiltering : int
	{
		Nearest = 0x2600,
		Linear  = 0x2601
	}

	public struct TextureImportSettings : IImportSettings
	{
		public bool HDR;
		public TextureFiltering Filtering;

		public TextureImportSettings(TextureFiltering filtering) : this(false, filtering) { }
		public TextureImportSettings(bool hdr, TextureFiltering filtering)
		{
			HDR = hdr;
			Filtering = filtering;
		}
	}

	[SerializeFileOptions(SaveSeparateFile = true)]
	public class Texture : NativeResourceBase, ISerializable
	{
		public bool HDR => m_ImportSettings.HDR;

		public IVector2 Resolution
		{
			get
			{
				_GetResolution(Handle, out IVector2 result);
				return result;
			}
		}

		public TextureFiltering Filter => m_ImportSettings.Filtering;

		private TextureImportSettings m_ImportSettings => (TextureImportSettings)ImportSettings;

		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			IntPtr handle;

			_Load(ResourcePath, out resourceID, out handle);
			ResourceID = resourceID;
			Handle = handle;

			ImportSettings = new TextureImportSettings()
			{
				HDR = _GetHDR(Handle),
				Filtering = (TextureFiltering)_GetFilter(Handle)
			};
		}

		protected override void OnImported()
		{
			if (TryGetImportSettings(out TextureImportSettings importSettings))
				ImportSettings = importSettings;
			else
				ImportSettings = new TextureImportSettings(TextureFiltering.Linear);
			Upload(VFS.Read(ResourcePath), (TextureImportSettings)ImportSettings);
		}

		public void Bind(uint index = 0) => _Bind(Handle, index);

		public void Upload(byte[] data, TextureImportSettings settings) =>
			_Upload(Handle, data, settings.HDR, (int)Filter);

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("HDR", HDR),
				new JProperty("Filtering", (int)Filter)
			);

		public void OnDeserialize(JObject json) =>
			Import(new TextureImportSettings()
			{
				HDR		  = json["HDR"].Value<bool>(),
				Filtering = (TextureFiltering)json["Filtering"].Value<int>()
			});

		public static (byte[], IVector2) Decode(VFSFile file, bool hdr = false)
		{
			bool success = _Decode(VFS.Read(file), hdr, out byte[] output, out IVector2 resolution);
			return success ? (output, resolution) : (new byte[0], IVector2.Zero);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Upload(IntPtr handle, byte[] data, bool hdr, int filter);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Bind(IntPtr handle, uint index);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPath(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetHDR(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetFilter(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetResolution(IntPtr handle, out IVector2 resolution);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Decode(byte[] fileData, bool hdr, out byte[] output, out IVector2 resolution);
		#endregion
	}
}