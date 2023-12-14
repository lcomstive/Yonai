using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanBuffer : IDisposable, IBuffer
	{
		public int BufferSize { get; private set; }

		public IntPtr BufferHandle { get; private set; }
		public IntPtr Allocation { get; private set; }

		private VulkanDevice m_Device;

		public VulkanBuffer(
			VulkanDevice device,
			int bufferSize,
			VkBufferUsage usage,
			VmaMemoryUsage memoryUsage)
		{
			m_Device = device;
			BufferSize = bufferSize;

			VkResult result = (VkResult)_Create(
				m_Device.Allocator,
				bufferSize,
				(int)usage,
				(int)memoryUsage,
				out IntPtr bufferHandle,
				out IntPtr allocation
			);
			BufferHandle = bufferHandle;
			Allocation = allocation;
			result.CheckForSuccess("Buffer creation");

			if(result != VkResult.Success)
			{
				Log.Error("Failed to create buffer");
				Application.Exit();
				return;
			}
		}

		public void Dispose() => _Destroy(m_Device.Allocator, BufferHandle, Allocation);

		public void Upload(byte[] data) =>
			_Upload(Allocation, data);

		public IntPtr MapMemory(int size, int offset = 0) => _MapMemory(m_Device.Device, Allocation, offset, size);
		public void UnmapMemory() => _UnmapMemory(m_Device.Device, Allocation);

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
		private static extern int _Create(IntPtr allocator, int size, int usage, int vmaUsage, out IntPtr bufferHandle, out IntPtr allocation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr allocator, IntPtr bufferHandle, IntPtr allocation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Upload(IntPtr allocation, byte[] data);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _MapMemory(IntPtr device, IntPtr memoryHandle, int offset, int size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _UnmapMemory(IntPtr device, IntPtr memoryHandle);

	}
}