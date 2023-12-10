using Yonai;
using System;
using Yonai.Systems;
using Yonai.Graphics;
using Yonai.Graphics.Backends.Vulkan;

namespace YonaiEditor.Systems
{
	public class ImGUISystem : YonaiSystem
	{
		private GraphicsAPI m_API = GraphicsAPI.None;
		private FunctionQueue m_DeletionQueue = new FunctionQueue();
		private IRenderPath m_PreviousRenderPath = null;
		private ImGUIRenderPath m_RenderPath;

		protected override void Start()
		{
			m_API = RenderSystem.API;

			switch(m_API)
			{
				default:
				case GraphicsAPI.None: return; // <-- intentional return
				case GraphicsAPI.Vulkan: VulkanInit(); break;
			}

			m_RenderPath = new ImGUIRenderPath();
			m_PreviousRenderPath = RenderSystem.Backend.RenderPath;
			RenderSystem.Backend.RenderPath = m_RenderPath;

			m_DeletionQueue.Enqueue(() => m_RenderPath.Dispose());
		}

		protected override void Destroyed()
		{
			m_DeletionQueue.Flush();

			if(RenderSystem.Backend.RenderPath is ImGUIRenderPath)
				RenderSystem.Backend.RenderPath = m_PreviousRenderPath;

			m_PreviousRenderPath = null;
		}

		#region Vulkan
		private VulkanDescriptorPool m_VulkanPool;

		private void VulkanInit()
		{
			VulkanGraphicsBackend backend = RenderSystem.Backend as VulkanGraphicsBackend;
			VulkanDevice device = backend.SelectedDevice;

			VkDescriptorPoolSize[] poolSizes = new VkDescriptorPoolSize[]
			{
				new VkDescriptorPoolSize { Type = VkDescriptorType.COMBINED_IMAGE_SAMPLER, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.SAMPLED_IMAGE, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.STORAGE_IMAGE, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.UNIFORM_TEXEL_BUFFER, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.STORAGE_TEXEL_BUFFER, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.UNIFORM_BUFFER, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.STORAGE_BUFFER, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.UNIFORM_BUFFER_DYNAMIC, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.STORAGE_BUFFER_DYNAMIC, DescriptorCount = 1000 },
				new VkDescriptorPoolSize { Type = VkDescriptorType.INPUT_ATTACHMENT, DescriptorCount = 1000 },
			};

			m_VulkanPool = new VulkanDescriptorPool(device, poolSizes, 1000);

			ImGUI.ImGui_ImplVulkan_InitInfo info = new ImGUI.ImGui_ImplVulkan_InitInfo
			{
				Instance = backend.Instance.Handle,
				PhysicalDevice = device.PhysicalDevice,
				LogicalDevice = device.Device,
				Queue = device.GraphicsQueue.Handle,
				DescriptorPool = m_VulkanPool.Handle,
				MinImageCount = 3,
				ImageCount = 3,
				UseDynamicRendering = true,
				ColorAttachmentFormat = backend.Swapchain.ImageFormat,
				MSAASamples = VkSampleCount._1
			};
			ImGUI.VulkanInit(ref info);

			backend.ImmediateSubmit(cmd => ImGUI.VulkanCreateFontsTexture(cmd.Handle));

			ImGUI.VulkanDestroyFontUploadObjects();

			m_DeletionQueue.Enqueue(() =>
			{
				m_VulkanPool.Dispose();
				ImGUI.VulkanShutdown();
			});
		}
		#endregion

		#region Render Path
		internal class ImGUIRenderPath : IRenderPath
		{
			public VulkanImage ColourOutput { get; private set; }

			private VulkanDevice m_Device;

			public ImGUIRenderPath()
			{
				m_Device = RenderSystem.Backend.Device as VulkanDevice;
				OnResized(Window.Resolution);
			}

			public void Draw(VulkanCommandBuffer cmd, Camera _)
			{
				// Transition image into drawable state
				cmd.TransitionImageLayout(ColourOutput, VkImageLayout.Undefined, VkImageLayout.GENERAL);

				VkRenderingAttachmentInfo attachmentInfo = new VkRenderingAttachmentInfo(
					ColourOutput,
					VkImageLayout.GENERAL,
					new VkClearValue(Colour.Black)
				);
				VkRenderingInfo renderInfo = new VkRenderingInfo
				{
					LayerCount = 1,
					RenderArea = new VkRect2D(IVector2.Zero, ColourOutput.Resolution),
					ColorAttachments = new VkRenderingAttachmentInfo[] { attachmentInfo }
				};
				cmd.BeginRendering(renderInfo);

				ImGUI.VulkanRender(cmd);

				cmd.EndRendering();
				
				// Transition image into transferrable state
				cmd.TransitionImageLayout(ColourOutput, VkImageLayout.GENERAL, VkImageLayout.TRANSFER_SRC_OPTIMAL);
		
				ImGUI.VulkanNewFrame();
			}

			public void OnResized(IVector2 resolution) => GenerateColourOutput(resolution);

			public void Dispose() => ColourOutput?.Dispose();

			private void GenerateColourOutput(IVector2 resolution)
			{
				ColourOutput?.Dispose();

				VkImageCreateInfo createInfo = VkImageCreateInfo.Default;
				createInfo.Format = VkFormat.B8G8R8A8_SRGB;
				createInfo.Extent = new Extents3D(resolution);
				createInfo.MipLevels = 1;
				createInfo.Usage = VkImageUsage.TransferSrc | VkImageUsage.TransferDst | VkImageUsage.ColorAttachment;

				VkImageViewCreateInfo imageViewInfo = VkImageViewCreateInfo.Default;
				imageViewInfo.Format = createInfo.Format;

				ColourOutput = new VulkanImage(m_Device, createInfo, imageViewInfo, VkSamplerCreateInfo.Default);
			}
		}
		#endregion
	}
}