using System;
using System.Runtime.InteropServices;
using Yonai._Internal;

namespace Yonai.Graphics.Backends.Vulkan
{
	[StructLayout(LayoutKind.Sequential)]
	public struct VkViewport
	{
		public float x, y, Width, Height, MinDepth, MaxDepth;

		public VkViewport(float x = 0, float y = 0, float width = 0, float height = 0, float minDepth = 0, float maxDepth = 1)
		{
			this.x = x;
			this.y = y;
			Width = width;
			Height = height;
			MinDepth = minDepth;
			MaxDepth = maxDepth;
		}

		public VkViewport(Vector2 offset, Vector2 size, Vector2 depthRange)
		{
			x = offset.x;
			y = offset.y;
			Width = size.x;
			Height = size.y;
			MinDepth = depthRange.x;
			MaxDepth = depthRange.y;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkRect2D
	{
		public IVector2 Offset;
		public Extents Extent;

		public VkRect2D(IVector2 offset, Extents extent)
		{
			Offset = offset;
			Extent = extent;
		}
	}

	#region Attachments
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
	#endregion

	#region Subpass
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
	#endregion

	#region Pipeline
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct VkPipelineShaderStage
	{
		private int _sType;
		private IntPtr _pNext;
		private int _flags;

		public VkShaderStage Stage;
		public IntPtr Shader;
		[MarshalAs(UnmanagedType.LPUTF8Str)]
		public string EntryPoint;

		private IntPtr _pSpecializationInfo;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkVertexInputBindingDescription
	{
		public uint Binding;
		public uint Stride;
		public VkVertexInputRate InputRate;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkVertexInputAttributeDescription
	{
		public uint Location;
		public uint Binding;
		public VkFormat Format;
		public uint Offset;
	}

	public struct VkPipelineVertexInputState
	{
		public VkVertexInputBindingDescription[] Bindings;
		public VkVertexInputAttributeDescription[] Attributes;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkPipelineVertexInputStateNative : IDisposable
	{
		private int _sType;
		private IntPtr _pNext;
		private int _flags;

		public uint BindingsCount;
		public IntPtr Bindings;

		public uint AttributesCount;
		public IntPtr Attributes;

		public VkPipelineVertexInputStateNative(VkPipelineVertexInputState state)
		{
			_sType = _flags = 0;
			_pNext = IntPtr.Zero;

			BindingsCount = (uint)(state.Bindings?.Length ?? 0);
			Bindings = InteropUtils.CreateNativeHandle(state.Bindings ?? new VkVertexInputBindingDescription[0]);

			AttributesCount = (uint)(state.Attributes?.Length ?? 0);
			Attributes = InteropUtils.CreateNativeHandle(state.Attributes ?? new VkVertexInputAttributeDescription[0]);
		}

		public void Dispose()
		{
			Marshal.FreeHGlobal(Bindings);
			Marshal.FreeHGlobal(Attributes);
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkPipelineInputAssemblyState
	{
		private int _sType;
		private IntPtr _pNext;
		private int _flags;

		public VkPrimitiveTopology Topology;
		public bool PrimitiveRestartEnable;
	}

	public struct VkPipelineRasterizationState
	{
		public bool DepthClampEnable;
		public bool RasterizerDiscardEnable;
		public VkPolygonMode PolygonMode;
		public VkCullMode CullMode;
		public VkFrontFace FrontFace;
		public bool DepthBiasEnable;
		public float DepthBiasConstantFactor;
		public float DepthBiasClamp;
		public float DepthBiasClampSlopeFactor;
		public float LineWidth;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct VkPipelineRasterizationStateNative
	{
		private int _sType;
		private IntPtr _pNext;
		private int _flags;

		public uint DepthClampEnable;
		public uint RasterizerDiscardEnable;
		public VkPolygonMode PolygonMode;
		public VkCullMode CullMode;
		public VkFrontFace FrontFace;
		public uint DepthBiasEnable;
		public float DepthBiasConstantFactor;
		public float DepthBiasClamp;
		public float DepthBiasClampSlopeFactor;
		public float LineWidth;

		public VkPipelineRasterizationStateNative(VkPipelineRasterizationState state)
		{
			_sType = _flags = 0;
			_pNext = IntPtr.Zero;

			DepthClampEnable = state.DepthClampEnable ? 1u : 0u;
			RasterizerDiscardEnable = state.RasterizerDiscardEnable ? 1u : 0u;
			PolygonMode = state.PolygonMode;
			CullMode = state.CullMode;
			FrontFace = state.FrontFace;
			DepthBiasEnable = state.DepthBiasEnable ? 1u : 0u;
			DepthBiasConstantFactor = state.DepthBiasConstantFactor;
			DepthBiasClamp = state.DepthBiasClamp;
			DepthBiasClampSlopeFactor = state.DepthBiasClampSlopeFactor;
			LineWidth = state.LineWidth;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkPipelineMultisampleState
	{
		private int _sType;
		private IntPtr _pNext;
		private int _flags;

		public VkSampleCount RasterizationSamples;
		private uint _SampleShadingEnable;
		public float MinSampleShading;
		private IntPtr _sampleMask;
		private uint _AlphaToCoverageEnable;
		private uint _AlphaToOneEnable;

		public bool SampleShadingEnable
		{
			get => _SampleShadingEnable != 0;
			set => _SampleShadingEnable = value ? 1u : 0u;
		}

		public bool AlphaToCoverageEnable
		{
			get => _AlphaToCoverageEnable != 0;
			set => _AlphaToCoverageEnable = value ? 1u : 0u;
		}

		public bool AlphaToOneEnable
		{
			get => _AlphaToOneEnable != 0;
			set => _AlphaToOneEnable = value ? 1u : 0u;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkPipelineColorBlendAttachment
	{
		private uint _BlendEnable;
		private int _SrcColorBlendFactor;
		private int _DstColorBlendFactor;
		private int _ColorBlendOp;
		private int _SrcAlphaBlendFactor;
		private int _DstAlphaBlendFactor;
		private int _AlphaBlendOp;
		private int _ColorWriteMask;

		public VkBlendFactor SrcColorBlendFactor
		{
			get => (VkBlendFactor)_SrcColorBlendFactor;
			set => _SrcColorBlendFactor = (int)value;
		}
		public VkBlendFactor DstColorBlendFactor
		{
			get => (VkBlendFactor)_DstColorBlendFactor;
			set => _DstColorBlendFactor = (int)value;
		}
		public VkBlendOp ColorBlendOp
		{
			get => (VkBlendOp)_ColorBlendOp;
			set => _ColorBlendOp = (int)value;
		}
		public VkBlendFactor SrcAlphaBlendFactor
		{
			get => (VkBlendFactor)_SrcAlphaBlendFactor;
			set => _SrcAlphaBlendFactor = (int)value;
		}
		public VkBlendFactor DstAlphaBlendFactor
		{
			get => (VkBlendFactor)_DstAlphaBlendFactor;
			set => _DstAlphaBlendFactor = (int)value;
		}
		public VkBlendOp AlphaBlendOp
		{
			get => (VkBlendOp)_AlphaBlendOp;
			set => _AlphaBlendOp = (int)value;
		}
		public VkColorComponentFlagBits ColorWriteMask
		{
			get => (VkColorComponentFlagBits)_ColorWriteMask;
			set => _ColorWriteMask = (int)value;
		}

		public bool BlendEnable
		{
			get => _BlendEnable != 0;
			set => _BlendEnable = value ? 1u : 0u;
		}
	}

	public struct VkPipelineColorBlendState
	{
		public bool LogicOpEnable;
		public VkLogicOp LogicOp;
		public VkPipelineColorBlendAttachment[] Attachments;
		public Vector4 BlendConstants;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct VkPipelineColorBlendStateNative : IDisposable
	{
		public float BlendConstantR;
		public float BlendConstantG;
		public float BlendConstantB;
		public float BlendConstantA;

		public bool LogicOpEnable;
		public VkLogicOp LogicOp;
		public uint AttachmentsCount;
		public IntPtr Attachments;

		public VkPipelineColorBlendStateNative(VkPipelineColorBlendState state)
		{
			LogicOp = state.LogicOp;
			LogicOpEnable = state.LogicOpEnable;

			AttachmentsCount = (uint)state.Attachments.Length;
			Attachments = InteropUtils.CreateNativeHandle(state.Attachments);

			BlendConstantR = state.BlendConstants.x;
			BlendConstantG = state.BlendConstants.y;
			BlendConstantB = state.BlendConstants.z;
			BlendConstantA = state.BlendConstants.w;
		}

		public void Dispose()
		{
			Marshal.FreeHGlobal(Attachments);
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkStencilOpState
	{
		public VkStencilOp failOp;
		public VkStencilOp passOp;
		public VkStencilOp depthFailOp;
		public VkCompareOp compareOp;
		public uint compareMask;
		public uint writeMask;
		public uint reference;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkPipelineDepthStencilState
	{
		private int _sType;
		private IntPtr _pNext;
		private int _flags;

		public bool DepthTestEnable;
		public bool DepthWriteEnable;
		public VkCompareOp DepthCompareOp;
		public bool DepthBoundsTestEnable;

		public bool StencilTestEnable;
		public VkStencilOpState Front;
		public VkStencilOpState Back;

		public float MinDepthBounds;
		public float MaxDepthBounds;
	}

	public struct VkGraphicsPipelineCreateInfo
	{
		public VkPipelineShaderStage[] Stages;
		public VkPipelineVertexInputState InputState;
		public VkPipelineInputAssemblyState InputAssembly;
		public VkViewport[] Viewports;
		public VkRect2D[] Scissors;
		public VkPipelineRasterizationState Rasterization;
		public VkPipelineMultisampleState Multisample;
		public VkPipelineDepthStencilState? DepthStencilState;
		public VkPipelineColorBlendState ColorBlendState;
		public VkDynamicState[] DynamicStates;

		public uint Subpass;
		public VulkanRenderPass RenderPass;

		public VulkanDescriptorSetLayout[] DescriptorSetLayouts;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct VkGraphicsPipelineCreateInfoNative : IDisposable
	{
		public uint StageCount;
		public IntPtr Stages;
		public VkPipelineVertexInputStateNative InputState;
		public VkPipelineInputAssemblyState InputAssembly;
		public uint ViewportsCount;
		public IntPtr Viewports;
		public uint ScissorsCount;
		public IntPtr Scissors;
		public VkPipelineRasterizationStateNative Rasterization;
		public VkPipelineMultisampleState Multisample;
		public IntPtr DepthStencilState;
		public VkPipelineColorBlendStateNative ColorBlendState;
		public uint DynamicStatesCount;
		[MarshalAs(UnmanagedType.LPArray)]
		public IntPtr DynamicStates;
		public IntPtr RenderPass;
		public uint Subpass;

		public uint DescriptorSetLayoutCount;
		public IntPtr DescriptorSetLayouts;

		public VkGraphicsPipelineCreateInfoNative(VulkanRenderPass renderPass, uint subpass, VkGraphicsPipelineCreateInfo info)
		{
			StageCount = (uint)info.Stages.Length;
			Stages = InteropUtils.CreateNativeHandle(info.Stages);
			InputState = new VkPipelineVertexInputStateNative(info.InputState);
			InputAssembly = info.InputAssembly;
			ViewportsCount = (uint)info.Viewports.Length;
			Viewports = InteropUtils.CreateNativeHandle(info.Viewports);
			ScissorsCount = (uint)info.Scissors.Length;
			Scissors = InteropUtils.CreateNativeHandle(info.Scissors);
			Rasterization = new VkPipelineRasterizationStateNative(info.Rasterization);
			Multisample = info.Multisample;
			ColorBlendState = new VkPipelineColorBlendStateNative(info.ColorBlendState);
			DynamicStatesCount = (uint)info.DynamicStates.Length;
			RenderPass = renderPass.Handle;
			Subpass = subpass;

			DynamicStates = Marshal.AllocHGlobal(sizeof(int) * info.DynamicStates.Length);
			IntPtr currPtr = DynamicStates;
			for (uint i = 0; i < DynamicStatesCount; i++, currPtr += sizeof(int))
				Marshal.StructureToPtr((int)info.DynamicStates[i], currPtr, false);

			if (info.DepthStencilState.HasValue)
				DepthStencilState = InteropUtils.CreateNativeHandle(info.DepthStencilState.Value);
			else
				DepthStencilState = IntPtr.Zero;

			DescriptorSetLayoutCount = (uint)info.DescriptorSetLayouts.Length;
			IntPtr[] descriptorHandles = new IntPtr[DescriptorSetLayoutCount];
			for(int i = 0;i < DescriptorSetLayoutCount;i++)
				descriptorHandles[i] = info.DescriptorSetLayouts[i].Handle;
			DescriptorSetLayouts = InteropUtils.CreateNativeHandle(descriptorHandles);
		}

		public void Dispose()
		{
			Marshal.FreeHGlobal(Stages);
			Marshal.FreeHGlobal(Scissors);
			Marshal.FreeHGlobal(Viewports);
			Marshal.FreeHGlobal(DynamicStates);
			Marshal.FreeHGlobal(DepthStencilState);
			Marshal.FreeHGlobal(DescriptorSetLayouts);

			InputState.Dispose();
			ColorBlendState.Dispose();
		}
	}

	public struct VkDescriptorSetLayoutBinding
	{
		public uint Binding;
		public VkDescriptorType DescriptorType;
		public uint DescriptorCount;
		public VkShaderStage StageFlags;
		private IntPtr _pImmutableSamplers;
	}
	#endregion
}