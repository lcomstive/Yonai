using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanBuffer : IDisposable
	{
		public int BufferSize { get; private set; }

		internal IntPtr BufferHandle;
		internal IntPtr MemoryHandle;
		private VulkanDevice m_Device;

		public VulkanBuffer(
			VulkanDevice device,
			int bufferSize,
			VkBufferUsage usage,
			VkMemoryProperty properties,
			VkSharingMode sharingMode = VkSharingMode.Exclusive)
		{
			m_Device = device;
			BufferSize = bufferSize;

			VkResult result = (VkResult)_Create(
				m_Device.Device,
				m_Device.PhysicalDevice,
				bufferSize,
				(int)usage,
				(int)properties,
				(int)sharingMode,
				out BufferHandle,
				out MemoryHandle
			);
			if(result != VkResult.VK_SUCCESS)
			{
				Log.Error("Failed to create buffer");
				Application.Exit();
				return;
			}
		}

		public void Dispose() => _Destroy(m_Device.Device, BufferHandle, MemoryHandle);

		public void Upload(byte[] data) =>
			_Upload(m_Device.Device, MemoryHandle, data);

		public IntPtr MapMemory(int size, int offset = 0) => _MapMemory(m_Device.Device, MemoryHandle, offset, size);
		public void UnmapMemory() => _UnmapMemory(m_Device.Device, MemoryHandle);

		/// <param name="size">Size, in bytes, of data to copy</param>
		public void CopyTo(VulkanCommandPool commandPool, VulkanBuffer other, int size)
		{
			VulkanCommandBuffer cmd = commandPool.CreateCommandBuffer(VkCommandBufferLevel.Primary);
			cmd.Begin(VkCommandBufferUsage.OneTimeSubmit);

			cmd.CopyBuffer(this, other, 0, 0, size);

			cmd.End();

			m_Device.GraphicsQueue.Submit(null, null, new VulkanCommandBuffer[] { cmd }, null, null);
			m_Device.GraphicsQueue.WaitIdle();

			cmd.Dispose();
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(IntPtr device, IntPtr physicalDevice, int bufferSize, int usage, int properties, int sharingMode, out IntPtr bufferHandle, out IntPtr memoryHandle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr bufferHandle, IntPtr memoryHandle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Upload(IntPtr device, IntPtr memoryHandle, byte[] data);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _MapMemory(IntPtr device, IntPtr memoryHandle, int offset, int size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _UnmapMemory(IntPtr device, IntPtr memoryHandle);

	}
}