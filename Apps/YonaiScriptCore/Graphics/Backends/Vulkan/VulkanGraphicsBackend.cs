using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

using static Yonai.Graphics.Mesh;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsBackend : IGraphicsBackend
	{
		public VulkanInstance Instance { get; private set; }

		public string[] AvailableExtensions { get; private set; }

		#region Temp
		public const int MaxFramesInFlight = 2;
		public VulkanSwapchain Swapchain { get; private set; } = null;
		public VulkanDevice SelectedDevice { get; private set; } = null;
		public VulkanRenderPass RenderPass { get; private set; } = null;
		public VulkanCommandPool CommandPool { get; private set; } = null;
		public VulkanGraphicsPipeline Pipeline { get; private set; } = null;
		public VulkanCommandBuffer[] CommandBuffers { get; private set; } = null;
		public VulkanFence[] InFlightFences { get; private set; } = new VulkanFence[0];
		public VulkanSemaphore[] ImageAvailableSemaphores { get; private set; } = new VulkanSemaphore[0];
		public VulkanSemaphore[] RenderFinishedSemaphores { get; private set; } = new VulkanSemaphore[0];
		public VulkanBuffer VertexBuffer { get; private set; }
		public VulkanBuffer IndexBuffer { get; private set; }

		// Uniform Buffers
		public IntPtr[] UniformBufferMemory { get; private set; } = new IntPtr[0];
		public VulkanBuffer[] UniformBuffers { get; private set; } = new VulkanBuffer[0];
		public VulkanDescriptorSetLayout DescriptorSet { get; private set; } = null;

		private struct ShaderMVP
		{
			public Matrix4 Model;
			public Matrix4 View;
			public Matrix4 Projection;
		}

		private bool m_FramebufferResized = false;

		private static readonly Vertex[] Vertices =
		{
				new Vertex
				{
					Position = new Vector3(-0.5f, -0.5f, 0),
					TexCoords = new Vector2(1, 0)
				},
				new Vertex
				{
					Position = new Vector3(0.5f, -0.5f, 0),
					TexCoords = new Vector2(0, 1)
				},
				new Vertex
				{
					Position = new Vector3(0.5f, 0.5f, 0),
					TexCoords = new Vector2(0.0f, 0.0f)
				},
				new Vertex
				{
					Position = new Vector3(-0.5f, 0.5f, 0),
					TexCoords = new Vector2(1.0f, 1.0f)
				},
		};
		private static readonly uint[] Indices =
		{
			0, 1, 2, 2, 3, 0
		};
		#endregion

		public void Create()
		{
			Log.Trace("Creating vulkan graphics backend");

			Instance = new VulkanInstance("Yonai Application", new Version(0, 0, 1));

			AvailableExtensions = _GetAvailableExtensions();
			Log.Debug($"Available extensions: ({AvailableExtensions.Length})");
			foreach (string extension in AvailableExtensions)
				Log.Debug($" - {extension}");

			#region Temp
			SelectedDevice = Instance.Devices[0];
			Log.Debug("\n\nChoosing first device - " + SelectedDevice.Name);

			Swapchain = SelectedDevice.CreateSwapchain();

			VkAttachmentDescription[] attachments = new VkAttachmentDescription[]
			{
				new VkAttachmentDescription()
				{
					Format = Swapchain.ImageFormat,
					Samples = VkSampleCount.BITS_1,
					LoadOp = VkAttachmentLoadOp.Clear,
					StoreOp = VkAttachmentStoreOp.Store,
					StencilLoadOp = VkAttachmentLoadOp.DontCare,
					StencilStoreOp = VkAttachmentStoreOp.DontCare,
					InitialLayout = VkImageLayout.UNDEFINED,
					FinalLayout = VkImageLayout.PRESENT_SRC_KHR
				}
			};
			VkSubpassDescription[] subpasses = new VkSubpassDescription[]
			{
				new VkSubpassDescription()
				{
					PipelineBindPoint = VkPipelineBindPoint.Graphics,
					ColourAttachments = new VkAttachmentReference[]
					{
						new VkAttachmentReference()
						{
							Attachment = 0,
							Layout = VkImageLayout.COLOR_ATTACHMENT_OPTIMAL
						}
					}
				}
			};
			VkSubpassDependency[] dependencies = new VkSubpassDependency[]
			{
				new VkSubpassDependency()
				{
					SrcSubpass = VkSubpassDependency.SubpassExternal,
					DstSubpass = 0,
					SrcStageMask = VkPipelineStageFlags.COLOR_ATTACHMENT_OUTPUT_BIT,
					SrcAccessMask = VkAccessFlags.NONE,
					DstStageMask = VkPipelineStageFlags.COLOR_ATTACHMENT_OUTPUT_BIT,
					DstAccessMask = VkAccessFlags.COLOR_ATTACHMENT_WRITE_BIT
				}
			};
			DescriptorSet = new VulkanDescriptorSetLayout(SelectedDevice, new VkDescriptorSetLayoutBinding[]
			{
				new VkDescriptorSetLayoutBinding
				{
					Binding = 0,
					DescriptorType = VkDescriptorType.UNIFORM_BUFFER,
					DescriptorCount = 1,
					StageFlags = VkShaderStage.Vertex
				}
			});
			RenderPass = new VulkanRenderPass(SelectedDevice, attachments, subpasses, dependencies);

			Swapchain.GenerateFramebuffers(RenderPass);

			CreateUniformBuffers();

			var pipelineInfo = CreatePipelineInfo();
			Pipeline = new VulkanGraphicsPipeline(pipelineInfo);
			CommandPool = new VulkanCommandPool(SelectedDevice);
			CommandBuffers = CommandPool.CreateCommandBuffers(MaxFramesInFlight, VkCommandBufferLevel.Primary);

			InFlightFences = new VulkanFence[MaxFramesInFlight];
			ImageAvailableSemaphores = new VulkanSemaphore[MaxFramesInFlight];
			RenderFinishedSemaphores = new VulkanSemaphore[MaxFramesInFlight];
			for (int i = 0; i < MaxFramesInFlight; i++)
			{
				InFlightFences[i] = new VulkanFence(SelectedDevice, VkFenceCreateFlags.Signaled);
				ImageAvailableSemaphores[i] = new VulkanSemaphore(SelectedDevice);
				RenderFinishedSemaphores[i] = new VulkanSemaphore(SelectedDevice);
			}

			CreateVertexBuffer();
			CreateIndexBuffer();

			Window.Resized += OnWindowResized;
			#endregion

			Log.Debug("Finished creating C# Vulkan backend");
		}

		private void OnWindowResized(IVector2 resolution) => m_FramebufferResized = true;

		private static uint CurrentFrame = 0;
		public void Draw()
		{
			InFlightFences[CurrentFrame].Wait();

			(VkResult result, uint imageIndex) = Swapchain.AcquireNextImage(ImageAvailableSemaphores[CurrentFrame], null);

			if (result == VkResult.VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapchain();
				return;
			}
			else if (result != VkResult.VK_SUCCESS && result != VkResult.VK_SUBOPTIMAL_KHR)
			{
				Log.Error("Failed to acquire swapchain image");
				Application.Exit();
				return;
			}

			InFlightFences[CurrentFrame].Reset();
			CommandBuffers[CurrentFrame].Reset();
			RecordCommandBuffer(CommandBuffers[CurrentFrame], imageIndex);

			UpdateUniformBuffer(imageIndex);

			VulkanSemaphore[] waitSemaphores = new VulkanSemaphore[] { ImageAvailableSemaphores[CurrentFrame] };
			VulkanSemaphore[] signalSemaphores = new VulkanSemaphore[] { RenderFinishedSemaphores[CurrentFrame] };
			VkPipelineStageFlags[] waitStages = new VkPipelineStageFlags[] { VkPipelineStageFlags.COLOR_ATTACHMENT_OUTPUT_BIT };
			VulkanCommandBuffer[] commandBuffers = new VulkanCommandBuffer[] { CommandBuffers[CurrentFrame] };

			result = SelectedDevice.GraphicsQueue.Submit(
				waitSemaphores, waitStages, commandBuffers, signalSemaphores, InFlightFences[CurrentFrame]);
			if (result != VkResult.VK_SUCCESS)
			{
				Application.Exit();
				return;
			}

			result = SelectedDevice.PresentQueue.Present(
				new VulkanSwapchain[] { Swapchain }, signalSemaphores, new uint[] { imageIndex });

			if (result == VkResult.VK_ERROR_OUT_OF_DATE_KHR ||
				result == VkResult.VK_SUBOPTIMAL_KHR || m_FramebufferResized)
				RecreateSwapchain();
			else if (result != VkResult.VK_SUCCESS)
			{
				Log.Error("Failed to present swapchain image");
				Application.Exit();
				return;
			}

			CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
		}

		private void RecreateSwapchain()
		{
			IVector2 res = Window.Resolution;
			while (res.x == 0 || res.y == 0)
			{
				Window.PollEvents();
				res = Window.Resolution;
			}

			m_FramebufferResized = false;
			Swapchain.Recreate();
		}

		private void RecordCommandBuffer(VulkanCommandBuffer cmd, uint imageIndex)
		{
			VkResult result = cmd.Begin();
			if(result != VkResult.VK_SUCCESS)
			{
				Log.Error("Failed to record command buffer");
				return;
			}

			cmd.BeginRenderPass(RenderPass, Swapchain.Framebuffers[imageIndex], IVector2.Zero, Swapchain.Resolution, Colour.Black);

			cmd.BindPipeline(Pipeline, VkPipelineBindPoint.Graphics);
			cmd.SetViewport(new VkViewport(offset: Vector2.Zero, size: Swapchain.Resolution, depthRange: new Vector2(0, 1)));
			cmd.SetScissor(new VkRect2D(offset: IVector2.Zero, new Extents(Swapchain.Resolution)));

			cmd.BindVertexBuffer(VertexBuffer);
			cmd.BindIndexBuffer(IndexBuffer);

			cmd.DrawIndexed((uint)Indices.Length);

			cmd.EndRenderPass();
			cmd.End();
		}

		private void CreateVertexBuffer()
		{
			int vertexSize = sizeof(float) * 8;
			int bufferSize = vertexSize * Vertices.Length;
			VulkanBuffer stagingBuffer = new VulkanBuffer(
				SelectedDevice,
				bufferSize,
				VkBufferUsage.TransferSource,
				VkMemoryProperty.HostVisible | VkMemoryProperty.HostCoherent
			);
			stagingBuffer.Upload(Vertices.ToByteArray());

			VertexBuffer = new VulkanBuffer(
				SelectedDevice,
				bufferSize,
				VkBufferUsage.TransferDestination | VkBufferUsage.Vertex,
				VkMemoryProperty.DeviceLocal
			);

			stagingBuffer.CopyTo(CommandPool, VertexBuffer, bufferSize);
			stagingBuffer.Dispose();
		}

		private void CreateIndexBuffer()
		{
			int bufferSize = sizeof(uint) * Indices.Length;
			byte[] indicesData = new byte[bufferSize];
			Buffer.BlockCopy(Indices, 0, indicesData, 0, bufferSize);

			VulkanBuffer stagingBuffer = new VulkanBuffer(
				SelectedDevice,
				bufferSize,
				VkBufferUsage.TransferSource,
				VkMemoryProperty.HostVisible | VkMemoryProperty.HostCoherent
			);
			stagingBuffer.Upload(indicesData);

			IndexBuffer = new VulkanBuffer(
				SelectedDevice,
				bufferSize,
				VkBufferUsage.TransferDestination | VkBufferUsage.Index,
				VkMemoryProperty.DeviceLocal
			);

			stagingBuffer.CopyTo(CommandPool, IndexBuffer, bufferSize);
			stagingBuffer.Dispose();
		}

		private void CreateUniformBuffers()
		{
			int bufferSize = Marshal.SizeOf(typeof(ShaderMVP));

			UniformBuffers = new VulkanBuffer[MaxFramesInFlight];
			UniformBufferMemory = new IntPtr[MaxFramesInFlight];
			for(int i = 0; i < MaxFramesInFlight;i++)
			{
				UniformBuffers[i] = new VulkanBuffer(
					SelectedDevice,
					bufferSize,
					VkBufferUsage.Uniform,
					VkMemoryProperty.HostVisible | VkMemoryProperty.HostCoherent
				);
				UniformBufferMemory[i] = UniformBuffers[i].MapMemory(bufferSize);
			}
		}

		public void Destroy()
		{
			Log.Trace("Destroying vulkan graphics backend");

			#region Temp
			for(int i = 0; i < MaxFramesInFlight; i++)
			{
				UniformBuffers[i].UnmapMemory();
				UniformBuffers[i].Dispose();
			}

			for (int i = 0; i < MaxFramesInFlight; i++)
			{
				InFlightFences[i].Dispose();
				ImageAvailableSemaphores[i].Dispose();
				RenderFinishedSemaphores[i].Dispose();
			}
			InFlightFences = new VulkanFence[0];
			ImageAvailableSemaphores = new VulkanSemaphore[0];
			RenderFinishedSemaphores = new VulkanSemaphore[0];

			DescriptorSet.Dispose();

			IndexBuffer.Dispose();
			VertexBuffer.Dispose();

			CommandPool.Dispose();
			Pipeline.Dispose();
			Swapchain.Dispose();

			SelectedDevice = null;
			#endregion

			Instance?.Dispose();
		}

		private void UpdateUniformBuffer(uint imageIndex)
		{
			ShaderMVP mvp;
			mvp.Model = Matrix4.Identity;
			mvp.View = Matrix4.LookAt(new Vector3(0, 0, -1), Vector3.Zero, Vector3.Up);
			mvp.Projection = Matrix4.Perspective(45.0f, Swapchain.Resolution.x / (float)Swapchain.Resolution.y);

			int matrixSize = sizeof(float) * 16;
			byte[] data = new byte[matrixSize * 3];
			Buffer.BlockCopy(mvp.Model.Components, 0, data, 0, matrixSize);
			Buffer.BlockCopy(mvp.View.Components, 0, data, matrixSize, matrixSize);
			Buffer.BlockCopy(mvp.Projection.Components, 0, data, matrixSize * 2, matrixSize);

			Marshal.Copy(data, 0, UniformBufferMemory[imageIndex], data.Length);
		}

		private VkGraphicsPipelineCreateInfo CreatePipelineInfo()
		{
			VulkanShaderModule[] shaders = new VulkanShaderModule[]
			{
				new VulkanShaderModule(SelectedDevice, "app://Assets/Shaders/vert.spv"),
				new VulkanShaderModule(SelectedDevice, "app://Assets/Shaders/frag.spv")
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

			uint v2Size = sizeof(float) * 2;
			uint v3Size = sizeof(float) * 3;
			VkPipelineVertexInputState vertexInput = new VkPipelineVertexInputState
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

			VkPipelineInputAssemblyState inputAssembly = new VkPipelineInputAssemblyState()
			{
				Topology = VkPrimitiveTopology.TRIANGLE_LIST,
				PrimitiveRestartEnable = false
			};

			VkRect2D scissor = new VkRect2D(IVector2.Zero, new Extents(Swapchain.Resolution));
			VkViewport viewport = new VkViewport(offset: Vector2.Zero, size: Swapchain.Resolution, new Vector2(0, 1));

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

			return new VkGraphicsPipelineCreateInfo
			{
				Stages = shaderStages,
				DescriptorSetLayouts = new VulkanDescriptorSetLayout[] { DescriptorSet },
				InputState = vertexInput,
				InputAssembly = inputAssembly,
				Viewports = new VkViewport[] { viewport },
				Scissors = new VkRect2D[] { scissor },
				Rasterization = rasterizer,
				Multisample = multisampling,
				ColorBlendState = colourBlending,
				DynamicStates = dynamicStates,
				RenderPass = RenderPass,
				Subpass = 0
			};
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern string[] _GetAvailableExtensions();
	}
}