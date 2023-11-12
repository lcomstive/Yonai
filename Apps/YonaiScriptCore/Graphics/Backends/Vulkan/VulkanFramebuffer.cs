using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanFramebuffer : IDisposable
	{
		internal IntPtr Handle;
		internal VulkanDevice Device;

		public VulkanFramebuffer(VulkanDevice device, VulkanRenderPass renderPass, VulkanImage attachment, IVector2 size) :
			this(device, renderPass, new VulkanImage[] { attachment }, size) { }
	
		public VulkanFramebuffer(VulkanDevice device, VulkanRenderPass renderPass, VulkanImage[] attachments, IVector2 size)
		{
			Device = device;

			IntPtr[] nativeAttachments = new IntPtr[attachments.Length];
			for (int i = 0; i < attachments.Length; i++)
				nativeAttachments[i] = attachments[i].ImageView;

			Handle = _Create(Device.Device,
				renderPass.Handle,
				nativeAttachments,
				(uint)size.x,
				(uint)size.y
			);
		}

		public void Dispose() => _Destroy(Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(
			IntPtr device,
			IntPtr renderPass,
			IntPtr[] attachments,
			uint width,
			uint height
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr framebuffer);
	}
}