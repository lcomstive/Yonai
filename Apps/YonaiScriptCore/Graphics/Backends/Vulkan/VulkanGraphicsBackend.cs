using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

using Yonai.IO;
using Yonai.Graphics.RenderPaths;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsBackend : IGraphicsBackend
	{
		public IGraphicsDevice Device => SelectedDevice;

		public VulkanInstance Instance { get; private set; }

		public string[] AvailableExtensions { get; private set; }

		private FunctionQueue DeletionQueue = new FunctionQueue();

		public const int MaxFramesInFlight = 2;
		public VulkanSwapchain Swapchain { get; private set; } = null;
		public VulkanDevice SelectedDevice { get; private set; } = null;
		public VulkanCommandPool CommandPool { get; private set; } = null;

		public IRenderPath RenderPath { get; private set; }

		// Offscreen buffer
		public VkSampleCount MSAASamples { get; private set; } = VkSampleCount._1;

		private bool m_FramebufferResized = false;

		public Model TestModel { get; private set; }
		public VulkanImage TestModelTexture { get; private set; }

		private class FrameData
		{
			public VulkanSemaphore SwapchainSemaphore, RenderSemaphore;
			public VulkanFence RenderFence;

			public VulkanCommandPool CommandPool;
			public VulkanCommandBuffer MainCommandBuffer;

			public FunctionQueue DeletionQueue;
		}

		private FrameData[] m_Frames;

		public void Create()
		{
			Log.Trace("Creating vulkan graphics backend");

			Instance = new VulkanInstance("Yonai Application", new Version(0, 0, 1));

			AvailableExtensions = _GetAvailableExtensions();
			Log.Debug($"Available extensions: ({AvailableExtensions.Length})");
			foreach (string extension in AvailableExtensions)
				Log.Debug($" - {extension}");

			SelectedDevice = Instance.Devices[0];
			MSAASamples = SelectedDevice.Limits.GetMaxSamples();
			Log.Debug("\n\nChoosing first device - " + SelectedDevice.Name);

			Swapchain = SelectedDevice.CreateSwapchain();
			CommandPool = new VulkanCommandPool(SelectedDevice);

			CreateTestModel();

			m_Frames = new FrameData[MaxFramesInFlight];
			for (int i = 0; i < MaxFramesInFlight; i++)
			{
				m_Frames[i] = new FrameData
				{
					RenderSemaphore = new VulkanSemaphore(SelectedDevice),
					SwapchainSemaphore = new VulkanSemaphore(SelectedDevice),

					RenderFence = new VulkanFence(SelectedDevice, VkFenceCreateFlags.Signaled),

					CommandPool = new VulkanCommandPool(SelectedDevice)
				};
				m_Frames[i].MainCommandBuffer = m_Frames[i].CommandPool.CreateCommandBuffer(VkCommandBufferLevel.Primary);
			}

			RenderPath = new ForwardRenderPath();
			RenderPath.OnResized(Window.Resolution);

			Window.Resized += OnWindowResized;

			Log.Debug("Finished creating C# Vulkan backend");
		}

		private void OnWindowResized(IVector2 resolution) => m_FramebufferResized = true;

		private static uint CurrentFrame = 0;
		public void Draw()
		{
			FrameData frame = m_Frames[CurrentFrame];
			const uint Timeout = 1000000000; // 1 second -> nanoseconds

			frame.RenderFence.Wait(Timeout);
			frame.RenderFence.Reset();

			(VkResult result, uint imageIndex) = Swapchain.AcquireNextImage(frame.SwapchainSemaphore, null, Timeout);
			if (result == VkResult.ErrorOutOfDate)
			{
				RecreateSwapchain();
				return;
			}
			else if (result != VkResult.Success && result != VkResult.Suboptimal)
			{
				Log.Error("Failed to acquire swapchain image");
				Application.Exit();
				return;
			}

			VulkanCommandBuffer cmd = frame.MainCommandBuffer;
			cmd.Reset();

			result = cmd.Begin(VkCommandBufferUsage.OneTimeSubmit);
			if(result != VkResult.Success)
			{
				Log.Error("Failed to begin command buffer");
				Application.Exit();
				return;
			}

			RenderPath.Draw(cmd, null);

			// Copy RenderPath.ColourOutput to swapchain image
			cmd.TransitionImageLayout(Swapchain.Images[imageIndex], VkImageLayout.Undefined, VkImageLayout.TRANSFER_DST_OPTIMAL);

			VkImageBlit blit = new VkImageBlit
			{
				SrcOffsets = new IVector3[] { IVector3.Zero, new IVector3(RenderPath.ColourOutput.Resolution, 1) },
				SrcSubresource = new VkImageSubresourceLayers
				{
					AspectMask = VkImageAspectFlags.Color,
					MipLevel = 0,
					BaseArrayLayer = 0,
					LayerCount = 1
				},

				DstOffsets = new IVector3[] { IVector3.Zero, new IVector3(Swapchain.Resolution, 1) },
				DstSubresource = new VkImageSubresourceLayers
				{
					AspectMask = VkImageAspectFlags.Color,
					MipLevel = 0,
					BaseArrayLayer = 0,
					LayerCount = 1
				}
			};

			// Copy render path output to swapchain image
			cmd.BlitImage(
				RenderPath.ColourOutput, VkImageLayout.TRANSFER_SRC_OPTIMAL,
				Swapchain.Images[imageIndex], VkImageLayout.TRANSFER_DST_OPTIMAL,
				new VkImageBlit[] { blit }, VkFilter.Nearest
			);

			// Transition swapchain image to presentable state
			cmd.TransitionImageLayout(Swapchain.Images[imageIndex], VkImageLayout.TRANSFER_DST_OPTIMAL, VkImageLayout.PRESENT_SRC_KHR);

			cmd.End();

			VulkanSemaphore[] signalSemaphores = new VulkanSemaphore[] { frame.RenderSemaphore };

			result = SelectedDevice.GraphicsQueue.Submit(
				waitSemaphores: new VulkanSemaphore[] { frame.SwapchainSemaphore },
				stageMask: new VkPipelineStageFlags[] { VkPipelineStageFlags.ColorAttachmentOutput },
				buffers: new VulkanCommandBuffer[] { cmd },
				signalSemaphores,
				frame.RenderFence
			);
			if (result != VkResult.Success)
			{
				Application.Exit();
				return;
			}

			result = SelectedDevice.GraphicsQueue.Present(Swapchain, signalSemaphores, imageIndex);
			if (result == VkResult.ErrorOutOfDate ||
				result == VkResult.Suboptimal || m_FramebufferResized)
				RecreateSwapchain();
			else if (result != VkResult.Success)
			{
				Log.Error("Failed to present swapchain image");
				Application.Exit();
				return;
			}

			CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
		}

		/// <summary>
		/// Handles window resizing, or swapchain becoming out of date
		/// </summary>
		private void RecreateSwapchain()
		{
			IVector2 res = Window.Resolution;
			while (res.x == 0 || res.y == 0)
			{
				Window.PollEvents();
				res = Window.Resolution;
			}

			SelectedDevice.WaitIdle();

			// Recreate
			m_FramebufferResized = false;
			Swapchain.Recreate();
			RenderPath.OnResized(Window.Resolution);
		}

		private void CreateTestModel()
		{
			TestModel = Resource.Load<Model>("app://TestModel/viking_room.obj");
			TestModelTexture = CreateTexture("app://TestModel/viking_room.png");
		}

		private VulkanImage CreateTexture(VFSFile filepath)
		{
			// Colour texture
			DecodedTexture decoded = Texture.Decode(filepath, false, 4);
			VulkanBuffer stagingBuffer = new VulkanBuffer(
				SelectedDevice,
				decoded.Data.Length,
				VkBufferUsage.TransferSource,
				VkMemoryProperty.HostVisible | VkMemoryProperty.HostCoherent
			);
			stagingBuffer.Upload(decoded.Data);

			VkImageCreateInfo imageInfo = VkImageCreateInfo.Default;
			imageInfo.Format = VkFormat.R8G8B8A8_SRGB;
			imageInfo.Usage = VkImageUsage.TransferSrc | VkImageUsage.TransferDst | VkImageUsage.Sampled;
			imageInfo.Extent = new Extents3D(decoded.Resolution);
			imageInfo.MipLevels = VulkanImage.CalculateMipLevels(decoded.Resolution);

			VkImageViewCreateInfo imageViewInfo = VkImageViewCreateInfo.Default;
			imageViewInfo.SubresourceRange.LevelCount = imageInfo.MipLevels;
			imageViewInfo.Format = imageInfo.Format;

			VkSamplerCreateInfo samplerInfo = VkSamplerCreateInfo.Default;
			samplerInfo.AnisotropyEnable = true;
			samplerInfo.MaxAnisotropy = SelectedDevice.Limits.MaxSamplerAnisotropy;
			samplerInfo.MaxLod = imageInfo.MipLevels;
			
			VulkanImage image = new VulkanImage(SelectedDevice, imageInfo, imageViewInfo, samplerInfo);

			CommandPool.TransitionImageLayout(image, VkImageLayout.Undefined, VkImageLayout.TRANSFER_DST_OPTIMAL);
			CommandPool.CopyBufferToImage(stagingBuffer, image, decoded.Resolution);
			// CommandPool.TransitionImageLayout(image, VkImageLayout.TRANSFER_DST_OPTIMAL, VkImageLayout.SHADER_READ_ONLY_OPTIMAL);

			image.GenerateMipmaps(CommandPool);

			stagingBuffer.Dispose();

			return image;
		}

		public void Destroy()
		{
			Log.Trace("Destroying vulkan graphics backend");

			#region Temp
			DeletionQueue.Flush();

			TestModelTexture.Dispose();

			for (int i = 0; i < MaxFramesInFlight; i++)
			{
				m_Frames[i].RenderFence.Dispose();
				m_Frames[i].RenderSemaphore.Dispose();
				m_Frames[i].SwapchainSemaphore.Dispose();

				m_Frames[i].MainCommandBuffer.Dispose();
				m_Frames[i].CommandPool.Dispose();
			}

			Swapchain.Dispose();
			CommandPool.Dispose();

			SelectedDevice = null;
			#endregion

			Instance?.Dispose();
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern string[] _GetAvailableExtensions();
	}
}