using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;
using Yonai.Systems;
using Yonai.Graphics.Backends.Vulkan;

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

	public struct DecodedTexture
	{
		public byte[] Data;
		public int Channels;
		public IVector2 Resolution;
	}

	[SerializeFileOptions(SaveSeparateFile = true)]
	public class Texture : ResourceBase, ISerializable
	{
		public bool HDR => m_ImportSettings.HDR;

		public IVector2 Resolution => m_Data.Resolution;

		public TextureFiltering Filter => m_ImportSettings.Filtering;

		private VkFormat m_Format = VkFormat.R8G8B8A8_SRGB;
		public VkFormat Format
		{
			get => m_Format;
			set 
			{
				if (m_Format == value) return;

				m_Format = value;
				GenerateInternalData();
			}
		}

		private ITexture m_Data;
		private IGraphicsDevice m_GraphicsDevice;
		private TextureImportSettings m_ImportSettings => (TextureImportSettings)ImportSettings;

		protected override void OnLoad() => m_GraphicsDevice = RenderSystem.Backend.Device;

		protected override void OnUnload() => m_Data?.Dispose();

		protected override void OnImported()
		{
			if (TryGetImportSettings(out TextureImportSettings importSettings))
				ImportSettings = importSettings;
			else
				ImportSettings = new TextureImportSettings(TextureFiltering.Linear);

			GenerateInternalData();
		}

		private void GenerateInternalData()
		{
			m_Data?.Dispose();

			DecodedTexture decoded = Decode(ResourcePath, m_ImportSettings.HDR, 4);
			m_Data = m_GraphicsDevice.CreateTexture(decoded.Data, Format, decoded.Resolution, 1.0f, false);
		}

		public void Upload(byte[] data) => m_Data?.Upload(data);

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

		public static DecodedTexture Decode(VFSFile file, bool hdr = false, int expectedChannels = 0)
		{
			bool success = _Decode(VFS.Read(file), hdr, expectedChannels, out byte[] output, out IVector2 resolution, out int channelCount);
			if (!success)
				return new DecodedTexture();
			return new DecodedTexture
			{
				Data = output,
				Resolution = resolution,
				Channels = channelCount
			};
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _Decode(
			byte[] fileData,
			bool hdr,
			int expectedChannels,
			out byte[] output,
			out IVector2 resolution,
			out int channelCount
		);
	}
}