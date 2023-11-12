using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsPipeline : IDisposable
	{
		internal IntPtr Handle;
		internal VulkanRenderPass RenderPass;
		private VulkanDevice Device => RenderPass.Device;

		public VkGraphicsPipelineCreateInfo PipelineCreateInfo { get; private set; }
		public uint Subpass { get; private set; }

		public VulkanGraphicsPipeline(VulkanRenderPass renderPass, uint subpass, Vector2 size)
		{
			RenderPass = renderPass;

			VulkanShaderModule[] shaders = new VulkanShaderModule[]
			{
				new VulkanShaderModule(Device, "app://Assets/Shaders/vert.spv"),
				new VulkanShaderModule(Device, "app://Assets/Shaders/frag.spv")
			};

			VkPipelineShaderStage[] shaderStages = new VkPipelineShaderStage[]
			{
				new VkPipelineShaderStage
				{
					Stage = VkShaderStage.Vertex,
					Shader = shaders[0].Handle,
					EntryPoint = "main"
				},
				new VkPipelineShaderStage
				{
					Stage = VkShaderStage.Fragment,
					Shader = shaders[1].Handle,
					EntryPoint = "main"
				}
			};

			uint v2Size = (uint)Marshal.SizeOf(typeof(Vector2));
			uint v3Size = (uint)Marshal.SizeOf(typeof(Vector3));
			VkPipelineVertexInputState vertexInput = new VkPipelineVertexInputState()
			{
				Bindings = new VkVertexInputBindingDescription[]
				{
					new VkVertexInputBindingDescription
					{
						Binding = 0,
						Stride = v3Size * 2 + v2Size,
						InputRate = VkVertexInputRate.Vertex
					}
				},
				Attributes = new VkVertexInputAttributeDescription[]
				{
					new VkVertexInputAttributeDescription
					{
						Binding = 0,
						Location = 0,
						Format = VkFormat.R32G32B32_SFLOAT,
						Offset = 0
					},
					new VkVertexInputAttributeDescription
					{
						Binding = 0,
						Location = 1,
						Format = VkFormat.R32G32B32_SFLOAT,
						Offset = v3Size
					},
					new VkVertexInputAttributeDescription
					{
						Binding = 0,
						Location = 2,
						Format = VkFormat.R32G32_SFLOAT,
						Offset = v3Size * 2
					}
				}
			};

			VkPipelineInputAssemblyState inputAssembly = new VkPipelineInputAssemblyState
			{
				Topology = VkPrimitiveTopology.TRIANGLE_LIST,
				PrimitiveRestartEnable = false
			};

			VkViewport viewport = new VkViewport
			{
				x = 0,
				y = 0,
				Width = size.x,
				Height = size.y,
				MinDepth = 0,
				MaxDepth = 1
			};

			VkRect2D scissor = new VkRect2D
			{
				Offset = IVector2.Zero,
				Extent = new Extents(size.x, size.y)
			};

			VkDynamicState[] dynamicStates = new VkDynamicState[]
			{
				VkDynamicState.VIEWPORT,
				VkDynamicState.SCISSOR
			};

			VkPipelineRasterizationState rasterizer = new VkPipelineRasterizationState
			{
				DepthClampEnable = false,
				RasterizerDiscardEnable = false,
				PolygonMode = VkPolygonMode.Fill,
				LineWidth = 1,
				CullMode = VkCullMode.BACK_BIT,
				FrontFace = VkFrontFace.Clockwise,
				DepthBiasEnable = false
			};

			VkPipelineMultisampleState multisampling = new VkPipelineMultisampleState
			{
				SampleShadingEnable = false,
				RasterizationSamples = VkSampleCount.BITS_1
			};

			VkPipelineColorBlendState colourBlending = new VkPipelineColorBlendState
			{
				LogicOpEnable = false,
				BlendConstants = new Vector4(1, 2, 3, 4),
				Attachments = new VkPipelineColorBlendAttachment[]
				{
					new VkPipelineColorBlendAttachment
					{
						ColorWriteMask = VkColorComponentFlagBits.R | VkColorComponentFlagBits.G | VkColorComponentFlagBits.B | VkColorComponentFlagBits.A,
						BlendEnable = false,
					}
				}
			};

			VkGraphicsPipelineCreateInfo pipelineInfo = new VkGraphicsPipelineCreateInfo
			{
				Stages = shaderStages,
				InputState = vertexInput,
				InputAssembly = inputAssembly,
				Viewports = new VkViewport[] { viewport },
				Scissors = new VkRect2D[] { scissor },
				Rasterization = rasterizer,
				Multisample = multisampling,
				ColorBlendState = colourBlending,
				DynamicStates = dynamicStates,
				RenderPass = renderPass,
				Subpass = subpass
			};

			Upload(subpass, pipelineInfo);
		}

		public void Dispose() => _Destroy(Handle);

		private void Upload(uint subpass, VkGraphicsPipelineCreateInfo pipelineInfo)
		{
			Subpass = subpass;
			PipelineCreateInfo = pipelineInfo;

			VkGraphicsPipelineCreateInfoNative native = new VkGraphicsPipelineCreateInfoNative(RenderPass, Subpass, PipelineCreateInfo);
			Handle = _Create(Device.Device, ref native);
			native.Dispose();
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(IntPtr device, ref VkGraphicsPipelineCreateInfoNative pipelineInfo);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr handle);
	}
}