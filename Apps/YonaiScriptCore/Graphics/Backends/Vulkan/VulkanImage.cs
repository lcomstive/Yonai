using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanImage : IDisposable
	{
		public VkFormat Format { get; private set; }
		internal IntPtr Image;
		internal IntPtr Sampler;
		internal IntPtr ImageView;
		internal IntPtr ImageMemory;
		private VulkanDevice m_Device;

		internal VulkanImage(IntPtr handle, int imageFormat, VulkanDevice device)
		{
			Image = handle;
			Format = (VkFormat)imageFormat;
			m_Device = device;

			ImageView = _CreateImageView(m_Device.Device, Image, imageFormat);
		}

		public VulkanImage(
			VulkanDevice device,
			VkImageType type,
			VkFormat format,
			VkImageUsage usage,
			VkImageTiling tiling,
			IVector2 resolution,
			VkSampleCount samples,
			int depth = 1,
			int mipLevels = 1,
			int arrayLayers = 1
			)
		{
			m_Device = device;
			Format = format;

			VkResult result = (VkResult)_Create(
				m_Device.Device,
				m_Device.PhysicalDevice,
				(int)type,
				(int)format,
				(int)usage,
				(int)samples,
				(int)tiling,
				resolution.x,
				resolution.y,
				depth,
				mipLevels,
				arrayLayers,
				out Image,
				out ImageMemory
			);

			ImageView = _CreateImageView(m_Device.Device, Image, (int)Format);

			VkSamplerCreateInfo samplerInfo = new VkSamplerCreateInfo
			{
				MagFilter = VkFilter.Linear,
				MinFilter = VkFilter.Linear,
				AddressModeU = VkSamplerAddressMode.Repeat,
				AddressModeV = VkSamplerAddressMode.Repeat,
				AddressModeW = VkSamplerAddressMode.Repeat,
				AnisotropyEnable = true,
				MaxAnisotropy = m_Device.Limits.MaxSamplerAnisotropy,
				BorderColor = VkBorderColor.IntOpaqueBlack,
				UnnormalizedCoordinates = false,
				CompareEnable = false,
				CompareOp = VkCompareOp.ALWAYS,
				MipmapMode = VkSamplerMipmapMode.Linear,
				MipLodBias = 0.0f,
				MinLod = 0.0f,
				MaxLod = 0.0f
			};
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
			int type,
			int format,
			int usage,
			int samples,
			int tiling,
			int width,
			int height,
			int depth,
			int mipLevels,
			int arrayLevels,
			out IntPtr handle,
			out IntPtr memoryHandle
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _CreateImageView(IntPtr device, IntPtr image, int imageFormat);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroyImageView(IntPtr device, IntPtr image);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CreateSampler(IntPtr device, ref VkSamplerCreateInfo info, out IntPtr sampler);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroySampler(IntPtr device, IntPtr sampler);
	}
}