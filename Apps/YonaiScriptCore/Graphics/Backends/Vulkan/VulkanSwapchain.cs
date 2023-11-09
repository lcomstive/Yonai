using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanSwapchain : IDisposable
	{
		public VkFormat ImageFormat { get; private set; }
		public VulkanImage[] Images { get; private set; }

		private IntPtr m_Handle;
		private VulkanDevice m_Device;
		private VulkanInstance m_Instance;

		internal VulkanSwapchain(VulkanDevice device, VulkanInstance instance)
		{
			m_Device = device;
			m_Instance = instance;
			m_Handle = _Create(
				device.PhysicalDevice, 
				device.Device, 
				instance.m_Surface, 
				IntPtr.Zero,
				out int imageFormat,
				out uint extentsWidth,
				out uint extentsHeight
			);

			ImageFormat = (VkFormat)imageFormat;

			IntPtr[] imageHandles = _GetImages(device.Device, m_Handle);
			Images = new VulkanImage[imageHandles.Length];
			for(int i = 0; i < imageHandles.Length; i++)
				Images[i] = new VulkanImage(imageHandles[i], imageFormat, m_Device);
		}

		public void Dispose() => _Destroy(m_Device.Device, m_Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(
			IntPtr physicalDevice,
			IntPtr device, 
			IntPtr surface,
			IntPtr oldSwapchain,
			out int imageFormat,
			out uint extentsWidth,
			out uint extentsHeight
		);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Destroy(IntPtr device, IntPtr swapchain);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr[] _GetImages(IntPtr device, IntPtr swapchain);
	}
}