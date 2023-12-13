using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanSwapchain : IDisposable
	{
		public VkFormat ImageFormat { get; private set; }
		public VulkanImage[] Images { get; private set; }
		public IVector2 Resolution { get; private set; }

		internal IntPtr Handle;
		private VulkanDevice m_Device;
		private VulkanInstance m_Instance;

		internal VulkanSwapchain(VulkanDevice device, VulkanInstance instance)
		{
			m_Device = device;
			m_Instance = instance;

			GenerateSwapchain();
		}

		private void GenerateSwapchain()
		{
			Handle = _Create(
				m_Device.PhysicalDevice,
				m_Device.Device,
				m_Instance.Surface,
				IntPtr.Zero,
				out int imageFormat,
				out uint extentsWidth,
				out uint extentsHeight
			);

			ImageFormat = (VkFormat)imageFormat;
			Resolution = new IVector2((int)extentsWidth, (int)extentsHeight);

			// Create image views
			IntPtr[] imageHandles = _GetImages(m_Device.Device, Handle);
			Images = new VulkanImage[imageHandles.Length];
			for (int i = 0; i < imageHandles.Length; i++)
				Images[i] = new VulkanImage(m_Device, imageHandles[i], imageFormat, Resolution);
		}

		public void Dispose()
		{
			foreach (VulkanImage image in Images)
			{
				image.DestroySampler();
				image.DestroyImageView();
			}

			_Destroy(m_Device.Device, Handle);
		}

		public void Recreate()
		{
			Dispose();
			GenerateSwapchain();
		}

		public (VkResult, uint) AcquireNextImage(VulkanSemaphore semaphore, VulkanFence fence = null, uint timeout = uint.MaxValue)
		{
			VkResult result = (VkResult)_AcquireNextImage(
				m_Device.Device,
				Handle,
				timeout,
				semaphore?.Handle ?? IntPtr.Zero,
				fence?.Handle ?? IntPtr.Zero,
				out uint imageIndex
			);
			return (result, imageIndex);
		}

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
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _AcquireNextImage(IntPtr device, IntPtr swapchain, uint timeout, IntPtr semaphore, IntPtr fence, out uint imageIndex);
	}
}