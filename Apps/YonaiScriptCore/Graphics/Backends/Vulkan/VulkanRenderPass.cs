using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	[StructLayout(LayoutKind.Sequential)]
	public struct VkAttachmentDescription
	{
		private int _flags;
		public VkFormat Format;
		public VkSampleCount Samples;
		public VkAttachmentLoadOp LoadOp;
		public VkAttachmentStoreOp StoreOp;
		public VkAttachmentLoadOp  StencilLoadOp;
		public VkAttachmentStoreOp StencilStoreOp;
		public VkImageLayout InitialLayout;
		public VkImageLayout FinalLayout;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkAttachmentReference
	{
		public uint Attachment;
		public VkImageLayout Layout;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkSubpassDescription
	{
		public VkSubpassDescriptionFlags Flags;
		public VkPipelineBindPoint PipelineBindPoint;
		public uint InputAttachmentCount;
		[MarshalAs(UnmanagedType.LPArray)]
		public VkAttachmentReference[] InputAttachments;
		public uint ColourAttachmentCount;
		public VkAttachmentReference[] ColourAttachments;
		public VkAttachmentReference[] ResolveAttachments;
		public VkAttachmentReference DepthStencilAttachment;
		public uint PreserveAttachmentCount;
		public uint[] PreserveAttachments;
	}

	public struct VkSubpassDependency
	{
		public uint SrcSubpass;
		public uint DstSubpass;
		public VkPipelineStageFlags SrcStageMask;
		public VkPipelineStageFlags DstStageMask;
		public VkAccessFlags SrcAccessMask;
		public VkAccessFlags DstAccessMask;
		public VkDependencyFlags DependencyFlags;

		public const uint SubpassExternal = ~0u;
	}

	public class VulkanRenderPass
	{
		internal IntPtr Handle;

		internal VulkanRenderPass(VulkanDevice device, VkAttachmentDescription[] attachments)
		{
			IntPtr cbase, ccur;

			int attachmentSize = Marshal.SizeOf(typeof(VkAttachmentDescription));
			cbase = Marshal.AllocHGlobal(attachmentSize * attachments.Length);
			ccur = cbase;

			for (int i = 0; i < attachments.Length; i++, ccur += attachmentSize)
				Marshal.StructureToPtr(attachments[i], ccur, false);

			Handle = _Create(
				device.Device,
				attachments.Length,
				cbase
			);

			Marshal.FreeHGlobal(cbase);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(
			IntPtr device,
			int attachmentCount,
			IntPtr attachments
		);
	}
}