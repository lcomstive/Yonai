using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using Yonai.Graphics.Backends.Vulkan;
using Yonai.Systems;

namespace Yonai.Graphics
{
	public struct ShaderImportSettings : IImportSettings
	{
		public Dictionary<VkShaderStage, VFSFile> StagePaths;
	}

	public class Shader : ResourceBase, ISerializable
	{
		public ShaderImportSettings ShaderStages
		{
			get => (ShaderImportSettings)ImportSettings;
			set => ImportSettings = value;
		}

		public Dictionary<VkShaderStage, IShaderModule> Modules { get; private set; }

		protected override void OnImported()
		{
			Modules = new Dictionary<VkShaderStage, IShaderModule>();

			if(!TryGetImportSettings(out ShaderImportSettings importSettings))
				return;

			ShaderStages = importSettings;

			IGraphicsDevice device = RenderSystem.Backend.Device;
			foreach(var pair in ShaderStages.StagePaths)
			{
				byte[] data = VFS.Read(pair.Value);
				Modules.Add(pair.Key, device.CreateShaderModule(data));
			}
		}

		public JObject OnSerialize()
		{
			JObject output = new JObject();

			foreach (var pair in ShaderStages.StagePaths)
			{
				string key = Enum.GetName(typeof(VkShaderStage), pair.Key);
				output.Add(new JProperty(key, pair.Value));
			}

			return output;
		}

		public void OnDeserialize(JObject json)
		{
			Dictionary<VkShaderStage, VFSFile> stagePaths = new Dictionary<VkShaderStage, VFSFile>();

			foreach (JProperty prop in json.Properties())
			{
				VkShaderStage stage = (VkShaderStage)Enum.Parse(typeof(VkShaderStage), prop.Name);
				stagePaths.Add(stage, prop.Value<string>());
			}

			Import(new ShaderImportSettings
			{
				StagePaths = stagePaths
			});
		}
	}
}