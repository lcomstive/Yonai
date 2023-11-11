using System;
using Yonai._Internal;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanRenderPass : IDisposable
	{
		internal IntPtr Handle;
		internal VulkanDevice Device;

		internal VulkanRenderPass(
			VulkanDevice device,
			VkAttachmentDescription[] attachments,
			VkSubpassDescription[] subpasses,
			VkSubpassDependency[] dependencies
			)
		{
			Device = device;

			VkSubpassDescriptionNative[] nativeSubpasses = new VkSubpassDescriptionNative[subpasses.Length];
			for (int i = 0; i < subpasses.Length; i++)
				nativeSubpasses[i] = new VkSubpassDescriptionNative(subpasses[i]);

			IntPtr ptrAttachments  = InteropUtils.CreateNativeHandle(attachments);
			IntPtr ptrSubpasses    = InteropUtils.CreateNativeHandle(nativeSubpasses);
			IntPtr ptrDependencies = InteropUtils.CreateNativeHandle(dependencies);

			Handle = _Create(
				Device.Device,
				attachments.Length,
				ptrAttachments,
				subpasses.Length,
				ptrSubpasses,
				dependencies.Length,
				ptrDependencies
			);

			Marshal.FreeHGlobal(ptrSubpasses);
			Marshal.FreeHGlobal(ptrAttachments);
			Marshal.FreeHGlobal(ptrDependencies);
		}

		public void Dispose() => _Destroy(Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(
			IntPtr device,
			int attachmentCount,
			IntPtr attachments,
			int subpassCount,
			IntPtr subpasses,
			int dependenciesCount,
			IntPtr dependencies
		);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr renderPass);
	}
}