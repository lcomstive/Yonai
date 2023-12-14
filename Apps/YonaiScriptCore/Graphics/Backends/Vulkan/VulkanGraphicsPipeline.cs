using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsPipeline : VulkanPipeline, IDisposable
	{
		private IntPtr m_Handle;
		internal override IntPtr Handle => m_Handle;

		private IntPtr m_PipelineLayout;
		internal override IntPtr PipelineLayout => m_PipelineLayout;

		internal VulkanRenderPass RenderPass;
		private VulkanDevice m_Device;

		public VkGraphicsPipelineCreateInfo PipelineCreateInfo { get; private set; }
		public uint Subpass { get; private set; }

		/// <summary>
		/// Creates a graphics pipeline object, using the <see cref="VulkanDevice"/>
		/// that created <see cref="VkGraphicsPipelineCreateInfo.RenderPass"/>
		/// </summary>
		public VulkanGraphicsPipeline(VkGraphicsPipelineCreateInfo info) :
			this(info.RenderPass?.Device, info) {}

		/// <summary>
		/// Creates a graphics pipeline object, with a manually specified <see cref="VulkanDevice"/>
		/// </summary>
		public VulkanGraphicsPipeline(VulkanDevice device, VkGraphicsPipelineCreateInfo info)
		{
			m_Device = device;
			RenderPass = info.RenderPass;
			Subpass = info.Subpass;
			Upload(Subpass, info);
		}

		public void Dispose() => _Destroy(Handle, PipelineLayout);

		private void Upload(uint subpass, VkGraphicsPipelineCreateInfo pipelineInfo)
		{
			Subpass = subpass;
			PipelineCreateInfo = pipelineInfo;

			if (m_Device == null)
				Log.Warning("VulkanGraphicsPipeline.Upload but device is null");

			VkGraphicsPipelineCreateInfoNative native = new VkGraphicsPipelineCreateInfoNative(RenderPass, Subpass, PipelineCreateInfo);
			m_Handle = _Create(m_Device.Device, ref native, out m_PipelineLayout);
			native.Dispose();
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(IntPtr device, ref VkGraphicsPipelineCreateInfoNative pipelineInfo, out IntPtr pipelineLayout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr handle, IntPtr pipelineLayout);
	}

	public class VulkanGraphicsPipelineBuilder
	{
		private VkGraphicsPipelineCreateInfo m_Info = Default;
		private VkPipelineRenderingCreateInfo m_RenderInfo = new VkPipelineRenderingCreateInfo();

		private List<VkFormat> m_ColourAttachmentFormats = new List<VkFormat>();
		private List<VkPipelineShaderStage> m_ShaderStages = new List<VkPipelineShaderStage>();

		private static readonly VkGraphicsPipelineCreateInfo Default = new VkGraphicsPipelineCreateInfo
		{
			Subpass = 0,
			Stages = null,
			Scissors = null,
			Viewports = null,
			RenderPass = null,
			DescriptorSetLayouts = null,
			InputState = new VkPipelineVertexInputState(),
			Multisample = new VkPipelineMultisampleState(),
			ColorBlendState = new VkPipelineColorBlendState(),
			InputAssembly = new VkPipelineInputAssemblyState(),
			Rasterization = new VkPipelineRasterizationState(),
			DepthStencilState = new VkPipelineDepthStencilState(),
			DynamicStates = new VkDynamicState[] { VkDynamicState.VIEWPORT, VkDynamicState.SCISSOR }
		};

		public void Clear()
		{
			m_Info = Default;
			m_ShaderStages.Clear();
			m_ColourAttachmentFormats.Clear();
			m_RenderInfo = new VkPipelineRenderingCreateInfo();
		}

		public VulkanGraphicsPipeline Build(VulkanDevice device)
		{
			// Insert some default viewport and scissor info.
			// These can be set later as they are dynamic
			m_Info.Viewports = new VkViewport[1];
			m_Info.Scissors = new VkRect2D[1];

			m_Info.RenderingInfo = m_RenderInfo;
			return new VulkanGraphicsPipeline(device, m_Info);
		}

		public VulkanGraphicsPipelineBuilder AddShader(IShaderModule module, VkShaderStage stage)
		{
			m_ShaderStages.Add(new VkPipelineShaderStage
			{
				EntryPoint = "main",
				Shader = module as VulkanShaderModule,
				Stage = stage
			});
			m_Info.Stages = m_ShaderStages.ToArray();

			return this;
		}

		public VulkanGraphicsPipelineBuilder SetInputTopology(VkPrimitiveTopology topology)
		{
			m_Info.InputAssembly.Topology = topology;
			m_Info.InputAssembly.PrimitiveRestartEnable = false;
			return this;
		}

		public VulkanGraphicsPipelineBuilder SetPolygonMode(VkPolygonMode mode)
		{
			m_Info.Rasterization.PolygonMode = mode;
			m_Info.Rasterization.LineWidth = 1.0f;

			return this;
		}

		public VulkanGraphicsPipelineBuilder SetCullMode(VkCullMode mode, VkFrontFace frontFace)
		{
			m_Info.Rasterization.CullMode = mode;
			m_Info.Rasterization.FrontFace = frontFace;

			return this;
		}

		public VulkanGraphicsPipelineBuilder SetMultisamplingNone()
		{
			m_Info.Multisample.SampleShadingEnable = false;
			m_Info.Multisample.RasterizationSamples = VkSampleCount._1;
			m_Info.Multisample.MinSampleShading = 1.0f;
			m_Info.Multisample.AlphaToCoverageEnable = false;
			m_Info.Multisample.AlphaToOneEnable = false;

			return this;
		}

		public VulkanGraphicsPipelineBuilder DisableBlending()
		{
			m_Info.ColorBlendState.Attachments = new VkPipelineColorBlendAttachment[]
			{
				new VkPipelineColorBlendAttachment
				{
					ColorWriteMask = VkColorComponentFlagBits.R | VkColorComponentFlagBits.G | VkColorComponentFlagBits.B | VkColorComponentFlagBits.A,
					BlendEnable = false
				}
			};

			return this;
		}

		public VulkanGraphicsPipelineBuilder AddColourAttachmentFormat(VkFormat format)
		{
			m_ColourAttachmentFormats.Add(format);
			m_RenderInfo.ColorAttachmentFormats = m_ColourAttachmentFormats.ToArray();

			return this;
		}

		public VulkanGraphicsPipelineBuilder SetDepthFormat(VkFormat format)
		{
			m_RenderInfo.DepthAttachmentFormat = format;
			return this;
		}

		public VulkanGraphicsPipelineBuilder SetStencilFormat(VkFormat format)
		{
			m_RenderInfo.StencilAttachmentFormat = format;
			return this;
		}

		public VulkanGraphicsPipelineBuilder DisableDepthTest()
		{
			m_Info.DepthStencilState = new VkPipelineDepthStencilState
			{
				DepthTestEnable = false,
				DepthWriteEnable = false,
				DepthCompareOp = VkCompareOp.NEVER,
				DepthBoundsTestEnable = false,
				StencilTestEnable = false,
				MinDepthBounds = 0,
				MaxDepthBounds = 1
			};

			return this;
		}
	}
}