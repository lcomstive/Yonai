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
		public VkAttachmentLoadOp StencilLoadOp;
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

	public struct VkSubpassDescription
	{
		public VkSubpassDescriptionFlags Flags;
		public VkPipelineBindPoint PipelineBindPoint;
		public VkAttachmentReference[] InputAttachments;
		public VkAttachmentReference[] ColourAttachments;
		public VkAttachmentReference[] ResolveAttachments;
		public VkAttachmentReference? DepthStencilAttachment;
		public uint[] PreserveAttachments;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct VkSubpassDescriptionNative
	{
		public VkSubpassDescriptionFlags Flags;
		public VkPipelineBindPoint PipelineBindPoint;
		public uint InputAttachmentCount;
		[MarshalAs(UnmanagedType.LPArray)]
		public VkAttachmentReference[] InputAttachments;
		public uint ColourAttachmentCount;
		[MarshalAs(UnmanagedType.LPArray)]
		public VkAttachmentReference[] ColourAttachments;
		[MarshalAs(UnmanagedType.LPArray)]
		public VkAttachmentReference[] ResolveAttachments;
		[MarshalAs(UnmanagedType.LPArray)]
		public VkAttachmentReference[] DepthStencilAttachment;
		public uint PreserveAttachmentCount;
		public uint[] PreserveAttachments;

		public VkSubpassDescriptionNative(VkSubpassDescription desc)
		{
			Flags = desc.Flags;
			PipelineBindPoint = desc.PipelineBindPoint;

			InputAttachments = desc.InputAttachments;
			InputAttachmentCount = (uint)(desc.InputAttachments?.Length ?? 0);

			ColourAttachments = desc.ColourAttachments;
			ColourAttachmentCount = (uint)(desc.ColourAttachments?.Length ?? 0);

			ResolveAttachments = desc.ResolveAttachments;

			if (desc.DepthStencilAttachment.HasValue)
				DepthStencilAttachment = new VkAttachmentReference[] { desc.DepthStencilAttachment.Value };
			else
				DepthStencilAttachment = null;

			PreserveAttachments = desc.PreserveAttachments;
			PreserveAttachmentCount = (uint)(desc.PreserveAttachments?.Length ?? 0);
		}
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
}