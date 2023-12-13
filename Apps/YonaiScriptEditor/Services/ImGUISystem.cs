using Yonai;
using Yonai.Systems;
using Yonai.Graphics;
using Yonai.Graphics.Backends.Vulkan;
using System.Collections.Generic;
using System;
using System.Runtime.CompilerServices;
using Yonai.Graphics.RenderPaths;

namespace YonaiEditor.Systems
{
	public class ImGUISystem : YonaiSystem
	{
		private static GraphicsAPI m_API = GraphicsAPI.None;

		private VulkanDevice m_Device;
		private FunctionQueue m_DeletionQueue = new FunctionQueue();
		internal static IRenderPath m_PreviousRenderPath = null;
		private ImGUIRenderPath m_RenderPath;

		private static Dictionary<ITexture, VulkanDescriptorSet> m_TextureDescriptors = new Dictionary<ITexture, VulkanDescriptorSet>();

		protected override void Start()
		{
			m_API = RenderSystem.API;

			switch(m_API)
			{
				default:
				case GraphicsAPI.None: return; // <-- intentional return
				case GraphicsAPI.Vulkan: VulkanInit(); break;
			}

			m_PreviousRenderPath = RenderSystem.Backend.RenderPath;
			m_RenderPath = new ImGUIRenderPath(m_PreviousRenderPath);
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

		internal static IntPtr GetTextureHandle(Texture texture) =>
			texture ? GetTextureHandle(texture.InternalData) : IntPtr.Zero;

		internal static IntPtr GetTextureHandle(ITexture texture)
		{
			if (texture == null) return IntPtr.Zero;

			switch(m_API)
			{
				default:
				case GraphicsAPI.None: return IntPtr.Zero;
				case GraphicsAPI.Vulkan:
					{
						VulkanImage image = texture as VulkanImage;
						if(image == null)
							return IntPtr.Zero;

						if (!m_TextureDescriptors.ContainsKey(image))
						{
							IntPtr handle = VulkanGenerateDescriptorSet(
								image.Sampler,
								image.ImageView,
								VkImageLayout.SHADER_READ_ONLY_OPTIMAL
							);
							VulkanDescriptorSet descriptorSet = VulkanDescriptorSet.FromHandle(handle);
							m_TextureDescriptors.Add(image, descriptorSet);
						}

						return m_TextureDescriptors[image].Handle;
					}
			}
		}

		#region Vulkan
		private VulkanDescriptorPool m_VulkanPool;

		private void VulkanInit()
		{
			VulkanGraphicsBackend backend = RenderSystem.Backend as VulkanGraphicsBackend;
			m_Device = backend.SelectedDevice;

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

			m_VulkanPool = new VulkanDescriptorPool(m_Device, poolSizes, 1000);

			ImGUI.ImGui_ImplVulkan_InitInfo info = new ImGUI.ImGui_ImplVulkan_InitInfo
			{
				Instance = backend.Instance.Handle,
				PhysicalDevice = m_Device.PhysicalDevice,
				LogicalDevice = m_Device.Device,
				Queue = m_Device.GraphicsQueue.Handle,
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

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr VulkanGenerateDescriptorSet(IntPtr sampler, IntPtr imageView, VkImageLayout layout);
		#endregion

		#region Render Path
		internal class ImGUIRenderPath : IRenderPath
		{
			public VulkanImage ColourOutput { get; private set; }

			private IRenderPath m_RenderPath;
			private VulkanDevice m_Device;

			public ImGUIRenderPath(IRenderPath renderPath)
			{
				m_Device = RenderSystem.Backend.Device as VulkanDevice;
				m_RenderPath = renderPath;

				OnResized(Window.Resolution);
			}

			public void Draw(VulkanCommandBuffer cmd)
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

				// Render into camera render textures
				m_PreviousRenderPath.Draw(cmd);
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