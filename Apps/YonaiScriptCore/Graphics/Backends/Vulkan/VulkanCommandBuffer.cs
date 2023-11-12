using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanCommandBuffer
	{
		internal IntPtr Handle;

		internal VulkanCommandBuffer(IntPtr handle) => Handle = handle;

		public VkResult Reset(VkCommandBufferResetFlag flag = VkCommandBufferResetFlag.None) =>
			(VkResult)_Reset(Handle, (int)flag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Reset(IntPtr handle, int flag);
	}
}
