using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Yonai._Internal;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanQueue
	{
		internal IntPtr Handle;
		internal uint Index;

		internal VulkanQueue(IntPtr handle, uint index)
		{
			Index = index;
			Handle = handle;
		}

		public VkResult Submit(
			VulkanSemaphore[] waitSemaphores,
			VkPipelineStageFlags[] stageMask,
			VulkanCommandBuffer[] buffers,
			VulkanSemaphore[] signalSemaphores, 
			VulkanFence fence)
		{
			IntPtr[] nativeWaitSemaphores = new IntPtr[waitSemaphores?.Length ?? 0];
			for (int i = 0; i < nativeWaitSemaphores.Length; i++)
				nativeWaitSemaphores[i] = waitSemaphores[i].Handle;

			IntPtr[] nativeBuffers = new IntPtr[buffers.Length];
			for (int i = 0; i < buffers.Length; i++)
				nativeBuffers[i] = buffers[i].Handle;

			IntPtr[] nativeSignalSemaphores = new IntPtr[signalSemaphores?.Length ?? 0];
			for (int i = 0; i < nativeSignalSemaphores.Length; i++)
				nativeSignalSemaphores[i] = signalSemaphores[i].Handle;

			VkResult result = (VkResult)_Submit(Handle, fence?.Handle ?? IntPtr.Zero, nativeWaitSemaphores, stageMask, nativeBuffers, nativeSignalSemaphores);
			return result;
		}

		public VkResult Present(VulkanSwapchain[] swapchains, VulkanSemaphore[] waitSemaphores, uint[] imageIndices)
		{
			IntPtr[] nativeWaitSemaphores = new IntPtr[waitSemaphores.Length];
			for (int i = 0; i < waitSemaphores.Length; i++)
				nativeWaitSemaphores[i] = waitSemaphores[i].Handle;

			IntPtr[] nativeSwapchains = new IntPtr[swapchains.Length];
			for (int i = 0; i < swapchains.Length; i++)
				nativeSwapchains[i] = swapchains[i].Handle;

			VkResult result = (VkResult)_Present(Handle, nativeWaitSemaphores, nativeSwapchains, imageIndices);

			return result;
		}

		public void WaitIdle() => _WaitIdle(Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Submit(IntPtr handle, IntPtr fence, IntPtr[] waitSemaphores, VkPipelineStageFlags[] stageMasks, IntPtr[] commandBuffers, IntPtr[] signalSemaphores);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Present(IntPtr handle, IntPtr[] waitSemaphores, IntPtr[] swapchains, uint[] imageIndices);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _WaitIdle(IntPtr handle);
	}
}