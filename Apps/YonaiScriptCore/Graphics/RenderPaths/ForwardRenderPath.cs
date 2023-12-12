using Yonai.Systems;
using Yonai.Graphics.Backends.Vulkan;
using System;

using PoolSizeRatio = Yonai.Graphics.Backends.Vulkan.VulkanDescriptorAllocator.PoolSizeRatio;

namespace Yonai.Graphics.RenderPaths
{
	public class ForwardRenderPath : IRenderPath
	{
		public VulkanImage ColourOutput { get; private set; }

		private VulkanDescriptorAllocator m_DescriptorAllocator;
		private VulkanDescriptorSet m_DrawDescriptors;
		private VulkanDescriptorSetLayout m_DrawDescriptorLayout;

		private VulkanDevice m_Device;
		private VulkanComputePipeline m_Pipeline;

		public ForwardRenderPath()
		{
			m_Device = RenderSystem.Backend.Device as VulkanDevice;
			m_DescriptorAllocator = new VulkanDescriptorAllocator(m_Device);

			InitDescriptors();
			CreatePipeline();

			OnResized(Window.Resolution);
		}

		public void Draw(VulkanCommandBuffer cmd)
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

			/*
			Colour clearColour = Colour.Black;
			clearColour.b = (float)Math.Abs(Math.Sin(Time.TimeSinceLaunch));
			cmd.ClearColorImage(ColourOutput, VkImageLayout.GENERAL, clearColour, clearRange);
			*/

			cmd.BindPipeline(m_Pipeline, VkPipelineBindPoint.Compute);
			cmd.BindDescriptorSets(m_Pipeline, VkPipelineBindPoint.Compute, m_DrawDescriptors);
			cmd.Dispatch((uint)Math.Ceiling(ColourOutput.Resolution.x / 16.0f), (uint)Math.Ceiling(ColourOutput.Resolution.y / 16.0f), 1);

			// Transition image into transferrable state
			cmd.TransitionImageLayout(ColourOutput, VkImageLayout.GENERAL, VkImageLayout.TRANSFER_SRC_OPTIMAL);

			CopyOutputToRenderTarget(cmd, Camera.Main?.RenderTarget as VulkanImage);
		}

		private void CopyOutputToRenderTarget(VulkanCommandBuffer cmd, VulkanImage target)
		{
			if (target == null)
				return;

			VkImageBlit blit = new VkImageBlit
			{
				SrcOffsets = new IVector3[] { IVector3.Zero, new IVector3(ColourOutput.Resolution, 1) },
				SrcSubresource = new VkImageSubresourceLayers
				{
					AspectMask = VkImageAspectFlags.Color,
					MipLevel = 0,
					BaseArrayLayer = 0,
					LayerCount = 1
				},

				DstOffsets = new IVector3[] { IVector3.Zero, new IVector3(target.Resolution, 1) },
				DstSubresource = new VkImageSubresourceLayers
				{
					AspectMask = VkImageAspectFlags.Color,
					MipLevel = 0,
					BaseArrayLayer = 0,
					LayerCount = 1
				}
			};
			
			cmd.TransitionImageLayout(target, VkImageLayout.Undefined, VkImageLayout.TRANSFER_DST_OPTIMAL);

			// Copy render path output to swapchain image
			cmd.BlitImage(
				ColourOutput, VkImageLayout.TRANSFER_SRC_OPTIMAL,
				target, VkImageLayout.TRANSFER_DST_OPTIMAL,
				new VkImageBlit[] { blit }, VkFilter.Nearest
			);

			// Transition target image to readable state
			cmd.TransitionImageLayout(target, VkImageLayout.TRANSFER_DST_OPTIMAL, VkImageLayout.SHADER_READ_ONLY_OPTIMAL);
		}

		public void OnResized(IVector2 resolution)
		{
			GenerateColourOutput(resolution);
			UpdateDescriptors();
		}

		public void Dispose()
		{
			ColourOutput?.Dispose();
			m_Pipeline?.Dispose();
		}

		private void GenerateColourOutput(IVector2 resolution)
		{
			ColourOutput?.Dispose();

			VkImageCreateInfo createInfo = VkImageCreateInfo.Default;
			createInfo.Format = VkFormat.R16G16B16A16_SFLOAT;
			createInfo.Extent = new Extents3D(resolution);
			createInfo.Usage = VkImageUsage.TransferSrc | VkImageUsage.TransferDst | VkImageUsage.Storage;

			VkImageViewCreateInfo imageViewInfo = VkImageViewCreateInfo.Default;
			imageViewInfo.Format = createInfo.Format;

			ColourOutput = new VulkanImage(m_Device, createInfo, imageViewInfo, VkSamplerCreateInfo.Default);
		}

		private void InitDescriptors()
		{
			PoolSizeRatio[] ratios = new PoolSizeRatio[]
			{
				new PoolSizeRatio
				{
					Type = VkDescriptorType.STORAGE_IMAGE,
					Ratio = 1.0f
				}
			};

			m_DescriptorAllocator.CreatePool(10, ratios);

			m_DrawDescriptorLayout = new VulkanDescriptorSetLayoutBuilder()
				.Add(0, VkDescriptorType.STORAGE_IMAGE, VkShaderStage.Compute)
				.Build(m_Device);

			m_DrawDescriptors = m_DescriptorAllocator.Allocate(m_DrawDescriptorLayout);
		}

		private void UpdateDescriptors()
		{
			VkWriteDescriptorSet drawImgWrite = new VkWriteDescriptorSet
			{
				DestinationBinding = 0,
				DescriptorCount = 1,
				DescriptorType = VkDescriptorType.STORAGE_IMAGE,
				ImageInfos = new VkDescriptorImageInfo[]
				{
					new VkDescriptorImageInfo
					{
						ImageLayout = VkImageLayout.GENERAL,
						ImageView = ColourOutput.ImageView
					}
				}
			};

			m_DrawDescriptors.Update(new VkWriteDescriptorSet[] { drawImgWrite });
		}

		private void CreatePipeline()
		{
			m_Pipeline?.Dispose();

			VulkanShaderModule shaderModule = new VulkanShaderModule(m_Device, "assets://Shaders/Vulkan/compute.spv");

			VkComputePipelineCreateInfo info = new VkComputePipelineCreateInfo
			{
				DescriptorSetLayouts = new VulkanDescriptorSetLayout[] { m_DrawDescriptorLayout },
				Shader = shaderModule
			};
			m_Pipeline = new VulkanComputePipeline(m_Device, info);
		}
	}
}