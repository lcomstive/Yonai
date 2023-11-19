using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanImage : IDisposable
	{
		public VkFormat Format { get; private set; }
		internal IntPtr Image;
		internal IntPtr ImageView;
		internal IntPtr ImageMemory;
		private VulkanDevice m_Device;

		internal VulkanImage(IntPtr handle, int imageFormat, VulkanDevice device)
		{
			Image = handle;
			Format = (VkFormat)imageFormat;
			m_Device = device;

			ImageView = _CreateImageView(Image, m_Device.Device, imageFormat);
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
		}

		public void Dispose() => _DestroyImageView(ImageView, m_Device.Device);

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
		private static extern IntPtr _CreateImageView(IntPtr image, IntPtr device, int imageFormat);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroyImageView(IntPtr image, IntPtr device);
	}
}