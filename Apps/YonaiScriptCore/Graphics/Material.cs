using Yonai.IO;
using Newtonsoft.Json.Linq;
using System;
using System.IO;
using System.Runtime.CompilerServices;
using Yonai.Graphics.Backends.Vulkan;
using Yonai.Systems;

namespace Yonai.Graphics
{
	public struct MaterialImportSettings : IImportSettings
	{
		public UUID Shader;
		public Colour Albedo;
		public UUID AlbedoMap;
		public bool AlphaClipping;
		public float AlphaThreshold;

		public MaterialImportSettings(UUID shader)
		{
			Shader = shader;
			Albedo = Colour.White;
			AlbedoMap = UUID.Invalid;
			AlphaClipping = false;
			AlphaThreshold = 0.1f;
		}
	}

	public class Material : ResourceBase, ISerializable
	{
		public bool SerializeAsCache => false;
		
		private MaterialImportSettings m_Settings;

		private Shader m_Shader = null;
		public Shader Shader
		{
			get => m_Shader;
			set
			{
				if (m_Shader == value)
					return; // No change

				m_Settings.Shader = value?.ResourceID ?? 0;
				// TODO: Recompile pipeline
			}
		}

		public Colour Albedo
		{
			get => m_Settings.Albedo;
			set
			{
				// Check for change
				if (m_Settings.Albedo == value)
					return;
				m_Settings.Albedo = value;
				// TODO: Update shader variable
			}
		}

		private Texture m_AlbedoMap = null;
		public Texture AlbedoMap
		{
			get => m_AlbedoMap;
			set
			{
				if (m_AlbedoMap == value)
					return; // No change

				m_Settings.AlbedoMap = value?.ResourceID ?? 0;
				// TODO: Update shader variable
			}
		}

		public bool AlphaClipping
		{
			get => m_Settings.AlphaClipping;
			set
			{
				// Check for change
				if (m_Settings.AlphaClipping == value)
					return;
				m_Settings.AlphaClipping = value;
				// TODO: Update shader variable
			}
		}

		public float AlphaThreshold
		{
			get => m_Settings.AlphaThreshold;
			set
			{
				// Check for change
				if (m_Settings.AlphaThreshold == value) return;
				
				m_Settings.AlphaThreshold = value;
				// TODO: Update shader variable
			}
		}

		private VulkanDevice m_Device;
		internal VulkanGraphicsPipeline m_Pipeline;

		protected override void OnLoad() => m_Device = (VulkanDevice)RenderSystem.Backend.Device;

		protected override void OnUnload() => m_Pipeline?.Dispose();

		protected override void OnImported()
		{
			if (!TryGetImportSettings(out m_Settings))
				// Reset to default values
				m_Settings = new MaterialImportSettings();

			m_Shader = Resource.Get<Shader>(m_Settings.Shader);
			m_AlbedoMap = Resource.Get<Texture>(m_Settings.AlbedoMap);

			// TODO: Create pipeline
			// TODO: Update shader variable
		}

		public JObject OnSerialize() =>
			new JObject(
					new JProperty("Shader", m_Settings.Shader.ToString()),
					new JProperty("Albedo", m_Settings.Albedo.OnSerialize()),
					new JProperty("AlbedoMap", m_Settings.AlbedoMap.ToString()),
					new JProperty("AlphaClipping", m_Settings.AlphaClipping),
					new JProperty("AlphaThreshold", m_Settings.AlphaThreshold)
				);

		public void OnDeserialize(JObject json)
		{
			MaterialImportSettings settings = new MaterialImportSettings()
			{
				Shader = ulong.Parse(json["Shader"].Value<string>()),
				AlbedoMap = ulong.Parse(json["AlbedoMap"].Value<string>()),
				AlphaClipping = json["AlphaClipping"].Value<bool>(),
				AlphaThreshold = json["AlphaThreshold"].Value<float>()
			};
			settings.Albedo.OnDeserialize(json["Albedo"].Value<JObject>());

			Import(settings);
		}

		private void GeneratePipeline()
		{
			m_Pipeline?.Dispose();

			VkGraphicsPipelineCreateInfo info = new VkGraphicsPipelineCreateInfo
			{
				
			};
			m_Pipeline = null; // new VulkanGraphicsPipeline(m_Device, info);
		}
	}
}

