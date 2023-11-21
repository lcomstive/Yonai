using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanImage : IDisposable
	{
		public VkFormat Format { get; private set; }
		public IVector2 Resolution { get; private set; }

		internal IntPtr Image;
		internal IntPtr Sampler;
		internal IntPtr ImageView;
		internal IntPtr ImageMemory;
		private VulkanDevice m_Device;

		private int m_MipLevels;

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

			_CreateSampler(m_Device.Device, ref samplerInfo, out Sampler);
		}

		public void Dispose()
		{
			_DestroyImageView(m_Device.Device, ImageView);
			_DestroySampler(m_Device.Device, Sampler);
		}

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