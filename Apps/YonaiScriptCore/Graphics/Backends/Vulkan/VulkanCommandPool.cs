using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanCommandPool : IDisposable
	{
		internal IntPtr Handle;
		internal VulkanDevice Device;

		public VulkanCommandPool(VulkanDevice device, VkCommandPoolCreateFlag flag = VkCommandPoolCreateFlag.ResetCommandBuffer) =>
			Handle = _Create(Device.Device, Device.GraphicsQueueIndex, (int)flag);

		public void Dispose() => _Destroy(Device.Device, Handle);

		public VulkanCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level)
		{
			_CreateCommandBuffers(Device.Device, Handle, 1u, (int)level, out IntPtr[] handles);
			return new VulkanCommandBuffer(handles[0]);
		}

		public VulkanCommandBuffer[] CreateCommandBuffers(int count, VkCommandBufferLevel level)
		{
			_CreateCommandBuffers(Device.Device, Handle, (uint)count, (int)level, out IntPtr[] handles);
			VulkanCommandBuffer[] buffers = new VulkanCommandBuffer[count];
			for(int i = 0; i < count; i++)
				buffers[i] = new VulkanCommandBuffer(Handle);
			return buffers;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(IntPtr device, uint graphicsQueueIndex, int flag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CreateCommandBuffers(
			IntPtr device,
			IntPtr commandPool,
			uint count,
			int bufferLevel,
			out IntPtr[] handles
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);
	}
}
