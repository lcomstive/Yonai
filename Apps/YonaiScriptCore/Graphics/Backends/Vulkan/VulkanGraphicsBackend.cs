using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsBackend : IGraphicsBackend
	{
		public VulkanInstance Instance { get; private set; }

		public string[] AvailableExtensions { get; private set; }

		#region Temp
		public const int MaxFramesInFlight = 2;
		public VulkanDevice SelectedDevice { get; private set; } = null;
		public VulkanSwapchain Swapchain { get; private set; } = null;
		public VulkanGraphicsPipeline Pipeline { get; private set; } = null;
		public VulkanCommandPool CommandPool { get; private set; } = null;
		public VulkanCommandBuffer[] CommandBuffers { get; private set; } = null;
		public VulkanFence[] InFlightFences { get; private set; } = new VulkanFence[0];
		public VulkanSemaphore[] ImageAvailableSemaphores { get; private set; } = new VulkanSemaphore[0];
		public VulkanSemaphore[] RenderFinishedSemaphores { get; private set; } = new VulkanSemaphore[0];
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
					PipelineBindPoint = VkPipelineBindPoint.GRAPHICS,
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
			VulkanRenderPass renderPass = new VulkanRenderPass(SelectedDevice, attachments, subpasses, dependencies);

			Swapchain.GenerateFramebuffers(renderPass);

			Pipeline = new VulkanGraphicsPipeline(renderPass, 0, Swapchain.Resolution);
			CommandPool = new VulkanCommandPool(SelectedDevice);
			CommandBuffers = CommandPool.CreateCommandBuffers(MaxFramesInFlight, VkCommandBufferLevel.Primary);

			InFlightFences = new VulkanFence[MaxFramesInFlight];
			ImageAvailableSemaphores = new VulkanSemaphore[MaxFramesInFlight];
			RenderFinishedSemaphores = new VulkanSemaphore[MaxFramesInFlight];
			for(int i = 0; i < MaxFramesInFlight; i++)
			{
				InFlightFences[i] = new VulkanFence(SelectedDevice, VkFenceCreateFlags.Signaled);
				ImageAvailableSemaphores[i] = new VulkanSemaphore(SelectedDevice);
				RenderFinishedSemaphores[i] = new VulkanSemaphore(SelectedDevice);
			}
			#endregion
		}

		public void Destroy()
		{
			Log.Trace("Destroying vulkan graphics backend");

			#region Temp
			for(int i = 0; i < MaxFramesInFlight; i++)
			{
				InFlightFences[i].Dispose();
				ImageAvailableSemaphores[i].Dispose();
				RenderFinishedSemaphores[i].Dispose();
			}
			InFlightFences = new VulkanFence[0];
			ImageAvailableSemaphores = new VulkanSemaphore[0];
			RenderFinishedSemaphores = new VulkanSemaphore[0];

			CommandPool.Dispose();
			Pipeline.Dispose();
			Swapchain.Dispose();

			SelectedDevice = null;
			#endregion

			Instance?.Dispose();
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern string[] _GetAvailableExtensions();

	}
}