using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanImage : IDisposable
	{
		public VkFormat Format { get; private set; }
		internal IntPtr Image;
		internal IntPtr ImageView;
		private VulkanDevice m_Device;

		internal VulkanImage(IntPtr handle, int imageFormat, VulkanDevice device)
		{
			Image = handle;
			Format = (VkFormat)imageFormat;
			m_Device = device;

			ImageView = _CreateImageView(Image, m_Device.Device, imageFormat);
		}

		public void Dispose() => _DestroyImageView(ImageView, m_Device.Device);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _CreateImageView(IntPtr image, IntPtr device, int imageFormat);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DestroyImageView(IntPtr image, IntPtr device);
	}
}