using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanImage : IDisposable
	{
		public VkFormat Format { get; private set; }
		public IVector2 Resolution { get; private set; }
		public uint MipLevels { get; private set; }

		internal IntPtr Image;
		internal IntPtr Sampler;
		internal IntPtr ImageView;
		internal IntPtr ImageMemory;
		private VulkanDevice m_Device;

		internal VulkanImage(VulkanDevice device, IntPtr handle, int imageFormat)
		{
			Image = handle;
			Format = (VkFormat)imageFormat;
			m_Device = device;

			VkImageViewCreateInfo imageViewInfo = new VkImageViewCreateInfo
			{
				m_Image = Image,
				ViewType = VkImageViewType._2D,
				Format = Format,
				Components = VkComponentMapping.Identity,
				SubresourceRange = new VkImageSubresourceRange
				{
					AspectMask = VkImageAspectFlags.Color,
					BaseMipLevel = 0,
					LevelCount = 1,
					BaseArrayLayer = 0,
					LayerCount = 1,
				}
			};

			MipLevels = 1;
			ImageView = _CreateImageView(m_Device.Device, Image, ref imageViewInfo);
		}

		public VulkanImage(
			VulkanDevice device,
			VkImageCreateInfo imageInfo,
			VkImageViewCreateInfo imageViewInfo,
			VkSamplerCreateInfo samplerInfo
		)
		{
			m_Device = device;
			Format = imageInfo.Format;
			Resolution = imageInfo.Extent;

			VkResult result = (VkResult)_Create(
				m_Device.Device,
				m_Device.PhysicalDevice,
				ref imageInfo,
				out Image,
				out ImageMemory
			);
			result.CheckForSuccess("Image creation");

			imageViewInfo.m_Image = Image;
			ImageView = _CreateImageView(m_Device.Device, Image, ref imageViewInfo);
			MipLevels = imageInfo.MipLevels;

			_CreateSampler(m_Device.Device, ref samplerInfo, out Sampler);
		}

		public void Dispose()
		{
			_DestroyImageView(m_Device.Device, ImageView);
			_DestroySampler(m_Device.Device, Sampler);
		}

		public void GenerateMipmaps(VulkanCommandPool commandPool)
		{
			VulkanCommandBuffer cmd = commandPool.BeginSingleTimeCommand();

			VkImageMemoryBarrier barrier = new VkImageMemoryBarrier
			{
				Image = this,
				SrcQueueFamilyIndex = VulkanQueue.Ignored,
				DstQueueFamilyIndex = VulkanQueue.Ignored,
				SubresourceRange = new VkImageSubresourceRange
				{
					AspectMask = VkImageAspectFlags.Color,
					BaseArrayLayer = 0,
					LayerCount = 1,
					LevelCount = 1
				}
			};

			int width = Resolution.x, height = Resolution.y;
			for(uint i = 1; i < MipLevels; i++)
			{
				barrier.SubresourceRange.BaseMipLevel = i - 1;
				barrier.OldLayout = VkImageLayout.TRANSFER_DST_OPTIMAL;
				barrier.NewLayout = VkImageLayout.TRANSFER_SRC_OPTIMAL;
				barrier.SrcAccessMask = VkAccessFlags.TransferWrite;
				barrier.DstAccessMask = VkAccessFlags.TransferRead;
				cmd.PipelineBarrier(barrier, VkPipelineStageFlags.Transfer, VkPipelineStageFlags.Transfer);

				VkImageBlit blit = new VkImageBlit
				{
					SrcOffsets = new IVector3[] { IVector3.Zero, new IVector3(width, height, 1) },
					SrcSubresource = new VkImageSubresourceLayers
					{
						AspectMask = VkImageAspectFlags.Color,
						MipLevel = i - 1,
						BaseArrayLayer = 0,
						LayerCount = 1
					},

					DstOffsets = new IVector3[]
					{
						IVector3.Zero,
						new IVector3(width > 1 ? width / 2 : 1, height > 1 ? height / 2 : 1, 1)
					},
					DstSubresource = new VkImageSubresourceLayers
					{
						AspectMask = VkImageAspectFlags.Color,
						MipLevel = i,
						BaseArrayLayer = 0,
						LayerCount = 1
					}
				};

				cmd.BlitImage(
					this, VkImageLayout.TRANSFER_SRC_OPTIMAL,
					this, VkImageLayout.TRANSFER_DST_OPTIMAL,
					new VkImageBlit[] { blit },
					VkFilter.Linear
				);

				barrier.OldLayout = VkImageLayout.TRANSFER_SRC_OPTIMAL;
				barrier.NewLayout = VkImageLayout.SHADER_READ_ONLY_OPTIMAL;
				barrier.SrcAccessMask = VkAccessFlags.TransferRead;
				barrier.DstAccessMask = VkAccessFlags.ShaderRead;
				cmd.PipelineBarrier(barrier, VkPipelineStageFlags.Transfer, VkPipelineStageFlags.FragmentShader);

				if (width > 1) width /= 2;
				if (height > 1) height /= 2;
			}

			barrier.SubresourceRange.BaseMipLevel = MipLevels - 1;
			barrier.OldLayout = VkImageLayout.TRANSFER_DST_OPTIMAL;
			barrier.NewLayout = VkImageLayout.SHADER_READ_ONLY_OPTIMAL;
			barrier.SrcAccessMask = VkAccessFlags.TransferWrite;
			barrier.DstAccessMask = VkAccessFlags.ShaderRead;
			cmd.PipelineBarrier(barrier, VkPipelineStageFlags.Transfer, VkPipelineStageFlags.FragmentShader);

			commandPool.EndSingleTimeCommand(cmd);
		}

		public static uint CalculateMipLevels(IVector2 resolution) =>
			(uint)Math.Floor(Math.Log(Math.Max(resolution.x, resolution.y), 2)) + 1;

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(
			IntPtr device,
			IntPtr physicalDevice,
			ref VkImageCreateInfo imageCreateInfo,
			out IntPtr handle,
			out IntPtr memoryHandle
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _CreateImageView(IntPtr device, IntPtr image, ref VkImageViewCreateInfo info);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroyImageView(IntPtr device, IntPtr image);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CreateSampler(IntPtr device, ref VkSamplerCreateInfo info, out IntPtr sampler);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroySampler(IntPtr device, IntPtr sampler);
	}
}