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

			DescriptorSetLayoutCount = (uint)(info.DescriptorSetLayouts?.Length ?? 0);
			IntPtr[] descriptorHandles = new IntPtr[DescriptorSetLayoutCount];
			for(int i = 0;i < DescriptorSetLayoutCount;i++)
				descriptorHandles[i] = info.DescriptorSetLayouts[i].Handle;
			DescriptorSetLayouts = DescriptorSetLayoutCount > 0 ?
				InteropUtils.CreateNativeHandle(descriptorHandles) : IntPtr.Zero;
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
		private readonly IntPtr _;
	}
	#endregion

	[StructLayout(LayoutKind.Sequential)]
	public struct VkImageSubresourceRange
	{
		public VkImageAspectFlags AspectMask;
		public uint BaseMipLevel;
		public uint LevelCount;
		public uint BaseArrayLayer;
		public uint LayerCount;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkImageMemoryBarrier
	{
		private int _sType;
		private IntPtr _pNext;
		public VkAccessFlags SrcAccessMask;
		public VkAccessFlags DstAccessMask;
		public VkImageLayout OldLayout;
		public VkImageLayout NewLayout;
		public uint SrcQueueFamilyIndex;
		public uint DstQueueFamilyIndex;

		private IntPtr m_Image;
		public VulkanImage Image { set => m_Image = value?.Image ?? IntPtr.Zero; }

		public VkImageSubresourceRange SubresourceRange;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkImageSubresourceLayers
	{
		public VkImageAspectFlags AspectMask;
		public uint MipLevel;
		public uint BaseArrayLayer;
		public uint LayerCount;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkBufferImageCopy
	{
		public ulong BufferOffset;
		public uint BufferRowLength;
		public uint BufferImageHeight;
		public VkImageSubresourceLayers ImageSubresource;
		public IVector3 ImageOffset;
		public Extents3D ImageExtent;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkPhysicalDeviceLimits
	{
		public uint MaxImageDimension1D;
		public uint MaxImageDimension2D;
		public uint MaxImageDimension3D;
		public uint MaxImageDimensionCube;
		public uint MaxImageArrayLayers;
		public uint MaxTexelBufferElements;
		public uint MaxUniformBufferRange;
		public uint MaxStorageBufferRange;
		public uint MaxPushConstantsSize;
		public uint MaxMemoryAllocationCount;
		public uint MaxSamplerAllocationCount;
		public ulong BufferImageGranularity;
		public ulong SparseAddressSpaceSize;
		public uint MaxBoundDescriptorSets;
		public uint MaxPerStageDescriptorSamplers;
		public uint MaxPerStageDescriptorUniformBuffers;
		public uint MaxPerStageDescriptorStorageBuffers;
		public uint MaxPerStageDescriptorSampledImages;
		public uint MaxPerStageDescriptorStorageImages;
		public uint MaxPerStageDescriptorInputAttachments;
		public uint MaxPerStageResources;
		public uint MaxDescriptorSetSamplers;
		public uint MaxDescriptorSetUniformBuffers;
		public uint MaxDescriptorSetUniformBuffersDynamic;
		public uint MaxDescriptorSetStorageBuffers;
		public uint MaxDescriptorSetStorageBuffersDynamic;
		public uint MaxDescriptorSetSampledImages;
		public uint MaxDescriptorSetStorageImages;
		public uint MaxDescriptorSetInputAttachments;
		public uint MaxVertexInputAttributes;
		public uint MaxVertexInputBindings;
		public uint MaxVertexInputAttributeOffset;
		public uint MaxVertexInputBindingStride;
		public uint MaxVertexOutputComponents;
		public uint MaxTessellationGenerationLevel;
		public uint MaxTessellationPatchSize;
		public uint MaxTessellationControlPerVertexInputComponents;
		public uint MaxTessellationControlPerVertexOutputComponents;
		public uint MaxTessellationControlPerPatchOutputComponents;
		public uint MaxTessellationControlTotalOutputComponents;
		public uint MaxTessellationEvaluationInputComponents;
		public uint MaxTessellationEvaluationOutputComponents;
		public uint MaxGeometryShaderInvocations;
		public uint MaxGeometryInputComponents;
		public uint MaxGeometryOutputComponents;
		public uint MaxGeometryOutputVertices;
		public uint MaxGeometryTotalOutputComponents;
		public uint MaxFragmentInputComponents;
		public uint MaxFragmentOutputAttachments;
		public uint MaxFragmentDualSrcAttachments;
		public uint MaxFragmentCombinedOutputResources;
		public uint MaxComputeSharedMemorySize;
		[MarshalAs(UnmanagedType.LPArray, SizeConst = 3, ArraySubType = UnmanagedType.SysUInt)]
		public uint[] MaxComputeWorkGroupCount;
		public uint MaxComputeWorkGroupInvocations;
		[MarshalAs(UnmanagedType.LPArray, SizeConst = 3, ArraySubType = UnmanagedType.SysUInt)]
		public uint[] MaxComputeWorkGroupSize;
		public uint SubPixelPrecisionBits;
		public uint SubTexelPrecisionBits;
		public uint MipmapPrecisionBits;
		public uint MaxDrawIndexedIndexValue;
		public uint MaxDrawIndirectCount;
		public float MaxSamplerLodBias;
		public float MaxSamplerAnisotropy;
		public uint MaxViewports;
		[MarshalAs(UnmanagedType.LPArray, SizeConst = 2, ArraySubType = UnmanagedType.SysUInt)]
		public uint[] MaxViewportDimensions;
		[MarshalAs(UnmanagedType.LPArray, SizeConst = 2, ArraySubType = UnmanagedType.R8)]
		public float[] ViewportBoundsRange;
		public uint ViewportSubPixelBits;
		public UIntPtr MinMemoryMapAlignment;
		public ulong MinTexelBufferOffsetAlignment;
		public ulong MinUniformBufferOffsetAlignment;
		public ulong MinStorageBufferOffsetAlignment;
		public int MinTexelOffset;
		public uint MaxTexelOffset;
		public int MinTexelGatherOffset;
		public uint MaxTexelGatherOffset;
		public float MinInterpolationOffset;
		public float MaxInterpolationOffset;
		public uint SubPixelInterpolationOffsetBits;
		public uint MaxFramebufferWidth;
		public uint MaxFramebufferHeight;
		public uint MaxFramebufferLayers;
		public VkSampleCount FramebufferColorSampleCounts;
		public VkSampleCount FramebufferDepthSampleCounts;
		public VkSampleCount FramebufferStencilSampleCounts;
		public VkSampleCount FramebufferNoAttachmentsSampleCounts;
		public uint MaxColorAttachments;
		public VkSampleCount SampledImageColorSampleCounts;
		public VkSampleCount SampledImageIntegerSampleCounts;
		public VkSampleCount SampledImageDepthSampleCounts;
		public VkSampleCount SampledImageStencilSampleCounts;
		public VkSampleCount StorageImageSampleCounts;
		public uint MaxSampleMaskWords;

		private uint m_TimestampComputeAndGraphics;
		public bool TimestampComputeAndGraphics
		{
			get => m_TimestampComputeAndGraphics == 1u;
			set => m_TimestampComputeAndGraphics = value ? 1u : 0u;
		}

		public float TimestampPeriod;
		public uint MaxClipDistances;
		public uint MaxCullDistances;
		public uint MaxCombinedClipAndCullDistances;
		public uint DiscreteQueuePriorities;
		[MarshalAs(UnmanagedType.LPArray, SizeConst = 2, ArraySubType = UnmanagedType.R8)]
		public float[] PointSizeRange;
		[MarshalAs(UnmanagedType.LPArray, SizeConst = 2, ArraySubType = UnmanagedType.R8)]
		public float[] LineWidthRange;
		public float PointSizeGranularity;
		public float LineWidthGranularity;

		private uint m_StandardSampleLocations;
		public bool StandardSampleLocations
		{
			get => m_StandardSampleLocations == 1u;
			set => m_StandardSampleLocations = value ? 1u : 0u;
		}

		private uint m_StrictLines;
		public bool StrictLines
		{
			get => m_StandardSampleLocations == 1u;
			set => m_StandardSampleLocations = value ? 1u : 0u;
		}

		public ulong OptimalBufferCopyOffsetAlignment;
		public ulong OptimalBufferCopyRowPitchAlignment;
		public ulong NonCoherentAtomSize;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkSamplerCreateInfo
	{
		private int _sType;
		private IntPtr _pNext;
		private int flags;

		public VkFilter MagFilter;
		public VkFilter MinFilter;
		public VkSamplerMipmapMode MipmapMode;
		public VkSamplerAddressMode AddressModeU;
		public VkSamplerAddressMode AddressModeV;
		public VkSamplerAddressMode AddressModeW;
		public float MipLodBias;

		private uint m_AnisotropyEnable;
		public bool AnisotropyEnable
		{
			get => m_AnisotropyEnable == 1u;
			set => m_AnisotropyEnable = value ? 1u : 0u;
		}

		public float MaxAnisotropy;

		private uint m_CompareEnable;
		public bool CompareEnable
		{
			get => m_CompareEnable == 1u;
			set => m_CompareEnable = value ? 1u : 0u;
		}

		public VkCompareOp CompareOp;
		public float MinLod;
		public float MaxLod;
		public VkBorderColor BorderColor;

		private uint m_UnnormalizedCoordinates;
		public bool UnnormalizedCoordinates
		{
			get => m_UnnormalizedCoordinates == 1u;
			set => m_UnnormalizedCoordinates = value ? 1u : 0u;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkDescriptorPoolSize
	{
		public VkDescriptorType Type;
		public uint DescriptorCount;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkDescriptorBufferInfo
	{
		public IntPtr Buffer;
		public ulong Offset;
		public ulong Range;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkDescriptorImageInfo
	{
		public IntPtr Sampler;
		public IntPtr ImageView;
		public VkImageLayout ImageLayout;
	}

	public struct VkWriteDescriptorSet
	{
		public uint DestinationBinding;
		public uint DestinationArrayElement;
		public uint DescriptorCount;
		public VkDescriptorType DescriptorType;
		public VkDescriptorImageInfo[] ImageInfos;
		public VkDescriptorBufferInfo[] BufferInfos;
		public IntPtr[] TexelBufferViews;
	}
	
	[StructLayout(LayoutKind.Sequential)]
	internal struct VkWriteDescriptorSetNative : IDisposable
	{
		private int _sType;
		private IntPtr _pNext;

		internal IntPtr DestinationSet;

		public uint DestinationBinding;
		public uint DestinationArrayElement;
		public uint DescriptorCount;
		public VkDescriptorType DescriptorType;
		public IntPtr ImageInfos;
		public IntPtr BufferInfos;
		public IntPtr TexelBufferViews;

		public VkWriteDescriptorSetNative(IntPtr dstSet, VkWriteDescriptorSet set)
		{
			_sType = 35; // VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET
			_pNext = IntPtr.Zero;

			DestinationSet = dstSet;

			DestinationBinding = set.DestinationBinding;
			DestinationArrayElement = set.DestinationArrayElement;
			DescriptorCount = set.DescriptorCount;
			DescriptorType = set.DescriptorType;
			ImageInfos = set.ImageInfos != null ? InteropUtils.CreateNativeHandle(set.ImageInfos) : IntPtr.Zero;
			BufferInfos = set.BufferInfos != null ? InteropUtils.CreateNativeHandle(set.BufferInfos) : IntPtr.Zero;
			TexelBufferViews = set.TexelBufferViews != null ? InteropUtils.CreateNativeHandle(set.TexelBufferViews) : IntPtr.Zero;
		}

		public void Dispose()
		{
			if (ImageInfos != IntPtr.Zero) Marshal.FreeHGlobal(ImageInfos);
			if (BufferInfos != IntPtr.Zero) Marshal.FreeHGlobal(BufferInfos);
			if (TexelBufferViews != IntPtr.Zero) Marshal.FreeHGlobal(TexelBufferViews);
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkImageCreateInfo
	{
		private int _sType;
		private IntPtr pNext;

		public VkImageCreateFlag Flags;
		public VkImageType ImageType;
		public VkFormat Format;
		public Extents3D Extent;
		public uint MipLevels;
		public uint ArrayLayers;
		public VkSampleCount Samples;
		public VkImageTiling Tiling;
		public VkImageUsage Usage;
		public VkSharingMode SharingMode;

		private uint m_QueueFamilyIndexCount;
		[MarshalAs(UnmanagedType.LPArray)]
		private uint[] m_QueueFamilyIndices;

		public uint[] QueueFamilyIndices
		{
			get => m_QueueFamilyIndices;
			set
			{
				m_QueueFamilyIndices = value;
				m_QueueFamilyIndexCount = (uint)(value?.Length ?? 0);
			}
		}

		public VkImageLayout InitialLayout;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkComponentMapping
	{
		public VkComponentSwizzle r;
		public VkComponentSwizzle g;
		public VkComponentSwizzle b;
		public VkComponentSwizzle a;

		public static VkComponentMapping Identity => new VkComponentMapping
		{
			r = VkComponentSwizzle.Identity,
			g = VkComponentSwizzle.Identity,
			b = VkComponentSwizzle.Identity,
			a = VkComponentSwizzle.Identity
		};
		
		public static VkComponentMapping Zero => new VkComponentMapping
		{
			r = VkComponentSwizzle.Zero,
			g = VkComponentSwizzle.Zero,
			b = VkComponentSwizzle.Zero,
			a = VkComponentSwizzle.Zero
		};
		
		public static VkComponentMapping One => new VkComponentMapping
		{
			r = VkComponentSwizzle.One,
			g = VkComponentSwizzle.One,
			b = VkComponentSwizzle.One,
			a = VkComponentSwizzle.One
		};
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkImageViewCreateInfo
	{
		private int _sType;
		private IntPtr _pNext;
		private int _flags;

		internal IntPtr m_Image;
		public VulkanImage Image { set => m_Image = value?.Image ?? IntPtr.Zero; }

		public VkImageViewType ViewType;
		public VkFormat Format;
		public VkComponentMapping Components;
		public VkImageSubresourceRange SubresourceRange;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkClearDepthStencilValue
	{
		public float Depth;
		public uint Stencil;
	}

	public struct VkClearValue
	{
		public Colour? Colour;
		public VkClearDepthStencilValue? DepthStencil;

		public VkClearValue(Colour colour)
		{
			Colour = colour;
			DepthStencil = null;
		}

		public VkClearValue(VkClearDepthStencilValue depthStencil)
		{
			Colour = null;
			DepthStencil = depthStencil;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct VkClearValueNative
	{
		public float ColorR;
		public float ColorG;
		public float ColorB;
		public float ColorA;

		public VkClearDepthStencilValue DepthStencil;

		public bool UseColour;

		public VkClearValueNative(VkClearValue value)
		{
			if (!value.Colour.HasValue && !value.DepthStencil.HasValue)
				throw new ArgumentException("VkClearValue needs either color or depth value");

			// Defaults
			ColorR = ColorG = ColorB = ColorA = 0.0f;
			DepthStencil = new VkClearDepthStencilValue();
			UseColour = value.Colour.HasValue;

			if (value.Colour.HasValue)
			{
				ColorR = value.Colour.Value.r;
				ColorG = value.Colour.Value.g;
				ColorB = value.Colour.Value.b;
				ColorA = value.Colour.Value.a;
			}
			else
				DepthStencil = value.DepthStencil.Value;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct VkImageBlit
	{
		public VkImageSubresourceLayers SrcSubresource;

		private IVector3 m_SrcOffset0;
		private IVector3 m_SrcOffset1;
		public IVector3[] SrcOffsets
		{
			set
			{
				m_SrcOffset0 = value?.Length > 0 ? value[0] : IVector3.Zero;
				m_SrcOffset1 = value?.Length > 1 ? value[1] : IVector3.Zero;
			}
		}

		public VkImageSubresourceLayers DstSubresource;

		private IVector3 m_DstOffset0;
		private IVector3 m_DstOffset1;
		public IVector3[] DstOffsets
		{
			set
			{
				m_DstOffset0 = value?.Length > 0 ? value[0] : IVector3.Zero;
				m_DstOffset1 = value?.Length > 1 ? value[1] : IVector3.Zero;
			}
		}

	}
}