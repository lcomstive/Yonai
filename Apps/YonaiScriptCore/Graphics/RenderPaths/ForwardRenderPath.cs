using Yonai.Systems;
using Yonai.Graphics.Backends.Vulkan;
using System;

namespace Yonai.Graphics.RenderPaths
{
	public class ForwardRenderPath : IRenderPath
	{
		public VulkanImage ColourOutput { get; private set; }

		public void Draw(VulkanCommandBuffer cmd, Camera camera)
		{
			// Transition image into drawable state
			cmd.TransitionImageLayout(ColourOutput, VkImageLayout.Undefined, VkImageLayout.GENERAL);

			VkImageSubresourceRange clearRange = new VkImageSubresourceRange
			{
				AspectMask = VkImageAspectFlags.Color,
				BaseArrayLayer = 0,
				BaseMipLevel = 0,
				LayerCount = 1,
				LevelCount = 1
			};

			Colour clearColour = Colour.Black;
			clearColour.b = (float)Math.Abs(Math.Sin(Time.TimeSinceLaunch));
			cmd.ClearColorImage(ColourOutput, VkImageLayout.GENERAL, clearColour, clearRange);

			// Transition image into transferrable state
			cmd.TransitionImageLayout(ColourOutput, VkImageLayout.GENERAL, VkImageLayout.TRANSFER_SRC_OPTIMAL);
		}

		public void OnResized(IVector2 resolution)
		{
			ColourOutput?.Dispose();

			VulkanDevice device = RenderSystem.Backend.Device as VulkanDevice;

			VkImageCreateInfo createInfo = VkImageCreateInfo.Default;
			createInfo.Format = VkFormat.R16G16B16A16_SFLOAT;
			createInfo.Extent = new Extents3D(Window.Resolution);
			createInfo.Usage = VkImageUsage.TransferSrc | VkImageUsage.TransferDst | VkImageUsage.Storage;

			VkImageViewCreateInfo imageViewInfo = VkImageViewCreateInfo.Default;
			imageViewInfo.Format = createInfo.Format;

			ColourOutput = new VulkanImage(device, createInfo, imageViewInfo, VkSamplerCreateInfo.Default);
		}

		public void Dispose() => ColourOutput.Dispose();
	}
}