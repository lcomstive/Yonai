using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanCommandPool : IDisposable
	{
		internal IntPtr Handle;
		internal VulkanDevice Device;

		public VulkanCommandPool(VulkanDevice device, VkCommandPoolCreateFlag flag = VkCommandPoolCreateFlag.ResetCommandBuffer)
		{
			Device = device;
			Handle = _Create(Device.Device, Device.GraphicsQueue.Index, (int)flag);
		}

		public void Dispose() => _Destroy(Device.Device, Handle);

		public VulkanCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level)
		{
			_CreateCommandBuffers(Device.Device, Handle, 1u, (int)level, out IntPtr[] handles);
			return new VulkanCommandBuffer(handles[0], this);
		}

		public VulkanCommandBuffer[] CreateCommandBuffers(int count, VkCommandBufferLevel level)
		{
			_CreateCommandBuffers(Device.Device, Handle, (uint)count, (int)level, out IntPtr[] handles);
			if(handles == null || handles.Length < count)
			{
				Log.Warning("Failed to create command buffers");
				return new VulkanCommandBuffer[0];
			}
			VulkanCommandBuffer[] buffers = new VulkanCommandBuffer[count];
			for(int i = 0;i < count;i++)
				buffers[i] = new VulkanCommandBuffer(handles[i], this);
			return buffers;
		}

		public VulkanCommandBuffer BeginSingleTimeCommand()
		{
			VulkanCommandBuffer buffer = CreateCommandBuffer(VkCommandBufferLevel.Primary);
			buffer.Begin(VkCommandBufferUsage.OneTimeSubmit);
			return buffer;
		}

		public void EndSingleTimeCommand(VulkanCommandBuffer buffer)
		{
			buffer.End();

			Device.GraphicsQueue.Submit(null, null, new VulkanCommandBuffer[] { buffer }, null, null);
			Device.GraphicsQueue.WaitIdle();

			buffer.Dispose();
		}

		public void TransitionImageLayout(VulkanImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			VulkanCommandBuffer buffer = BeginSingleTimeCommand();

			VkImageMemoryBarrier barrier = new VkImageMemoryBarrier
			{
				OldLayout = oldLayout,
				NewLayout = newLayout,

				SrcQueueFamilyIndex = VulkanQueue.Ignored,
				DstQueueFamilyIndex = VulkanQueue.Ignored,

				Image = image,

				SubresourceRange = new VkImageSubresourceRange
				{
					AspectMask = VkImageAspectFlags.Color,
					BaseMipLevel = 0,
					LevelCount = 1,
					BaseArrayLayer = 0,
					LayerCount = 1,
				}
			};

			VkPipelineStageFlags sourceStage = VkPipelineStageFlags.None;
			VkPipelineStageFlags destinationStage = VkPipelineStageFlags.None;
			if(oldLayout == VkImageLayout.UNDEFINED && newLayout == VkImageLayout.TRANSFER_DST_OPTIMAL)
			{
				barrier.SrcAccessMask = 0;
				barrier.DstAccessMask = VkAccessFlags.TransferWrite;

				sourceStage = VkPipelineStageFlags.TopOfPipe;
				destinationStage = VkPipelineStageFlags.Transfer;
			}
			else if(oldLayout == VkImageLayout.TRANSFER_DST_OPTIMAL && newLayout == VkImageLayout.SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.SrcAccessMask = VkAccessFlags.TransferWrite;
				barrier.DstAccessMask = VkAccessFlags.ShaderRead;

				sourceStage = VkPipelineStageFlags.Transfer;
				destinationStage = VkPipelineStageFlags.FragmentShader;
			}

			buffer.PipelineBarrier(barrier, sourceStage, destinationStage);

			EndSingleTimeCommand(buffer);
		}

		public void CopyBufferToImage(VulkanBuffer buffer, VulkanImage image, IVector2 resolution)
		{
			VulkanCommandBuffer cmd = BeginSingleTimeCommand();

			VkBufferImageCopy region = new VkBufferImageCopy
			{
				BufferOffset = 0,
				BufferRowLength = 0,
				BufferImageHeight = 0,

				ImageSubresource = new VkImageSubresourceLayers
				{
					AspectMask = VkImageAspectFlags.Color,
					MipLevel = 0,
					BaseArrayLayer = 0,
					LayerCount = 1
				},

				ImageOffset = IVector3.Zero,
				ImageExtent = new Extents3D(resolution)
			};
			cmd.CopyBufferToImage(buffer, image, region);

			EndSingleTimeCommand(cmd);
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
