using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanImage
	{
		public VkFormat Format { get; private set; }
		internal IntPtr Image;
		internal IntPtr ImageView;

		internal VulkanImage(IntPtr handle, int imageFormat, VulkanDevice device)
		{
			Image = handle;
			Format = (VkFormat)imageFormat;

			ImageView = _CreateImageView(Image, device.Device, imageFormat);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _CreateImageView(IntPtr image, IntPtr device, int imageFormat);
	}
}