using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanImage : ITexture
	{
		public VkFormat Format { get; private set; }
		public IVector2 Resolution { get; private set; }
		public uint MipLevels { get; private set; }

		public IntPtr Image { get; private set; }
		public IntPtr Sampler { get; private set; }
		public IntPtr ImageView { get; private set; }
		public IntPtr ImageMemory { get; private set; }

		private VulkanDevice m_Device;

		internal VulkanImage(VulkanDevice device, IntPtr handle, int imageFormat, IVector2 resolution)
		{
			Image = handle;
			m_Device = device;
			Resolution = resolution;
			Format = (VkFormat)imageFormat;

			VkImageViewCreateInfo imageViewInfo = new VkImageViewCreateInfo
			{
				m_Image = Image,
				ViewType = VkImageViewType._2D,
				Format = Format,
				Components = VkComponentMapping.Zero,
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
				m_Device.Allocator,
				ref imageInfo,
				out IntPtr image,
				out IntPtr imageMemory
			);
			result.CheckForSuccess("Image creation");

			Image = image;
			ImageMemory = imageMemory;

			imageViewInfo.m_Image = Image;
			ImageView = _CreateImageView(m_Device.Device, Image, ref imageViewInfo);
			MipLevels = imageInfo.MipLevels;

			_CreateSampler(m_Device.Device, ref samplerInfo, out IntPtr sampler);
			Sampler = sampler;
		}

		public void Dispose()
		{
			DestroyImageView();
			DestroySampler();
			_Destroy(m_Device.Allocator, Image, ImageMemory);
		}

		internal void DestroySampler()
		{
			if(Sampler != null && Sampler != IntPtr.Zero)
				_DestroySampler(m_Device.Device, Sampler);
			Sampler = IntPtr.Zero;
		}

		internal void DestroyImageView()
		{
			if(ImageView != null && ImageView != IntPtr.Zero)
				_DestroyImageView(m_Device.Device, ImageView);
			ImageView = IntPtr.Zero;
		}

		public void GenerateMipmaps() => GenerateMipmaps(m_Device.m_Backend.CommandPool);

		public void GenerateMipmaps(VulkanCommandPool commandPool)
		{
			commandPool.TransitionImageLayout(this, VkImageLayout.SHADER_READ_ONLY_OPTIMAL, VkImageLayout.TRANSFER_DST_OPTIMAL);
			
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

		public void TransitionImageLayout(VkImageLayout from, VkImageLayout to) =>
			m_Device.m_Backend.CommandPool.TransitionImageLayout(this, from, to);

		public void Upload(byte[] data) => Upload(data, m_Device.m_Backend.CommandPool);

		public void Upload(byte[] data, VulkanCommandPool commandPool, VkImageLayout outputLayout = VkImageLayout.SHADER_READ_ONLY_OPTIMAL)
		{
			VulkanBuffer stagingBuffer = new VulkanBuffer(
				m_Device,
				data.Length,
				VkBufferUsage.TransferSource,
				VmaMemoryUsage.CPU_ONLY
			);
			stagingBuffer.Upload(data);

			commandPool.TransitionImageLayout(this, VkImageLayout.Undefined, VkImageLayout.TRANSFER_DST_OPTIMAL);
			commandPool.CopyBufferToImage(stagingBuffer, this, Resolution);

			if(outputLayout != VkImageLayout.TRANSFER_DST_OPTIMAL)
				commandPool.TransitionImageLayout(this, VkImageLayout.TRANSFER_DST_OPTIMAL, outputLayout);

			stagingBuffer.Dispose();
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(
			IntPtr device,
			IntPtr allocator,
			ref VkImageCreateInfo imageCreateInfo,
			out IntPtr handle,
			out IntPtr allocation
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr allocator, IntPtr image, IntPtr memoryAllocation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _CreateImageView(IntPtr device, IntPtr image, ref VkImageViewCreateInfo info);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroyImageView(IntPtr device, IntPtr image);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CreateSampler(IntPtr device, ref VkSamplerCreateInfo info, out IntPtr sampler);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroySampler(IntPtr device, IntPtr sampler);
		#endregion
	}
}