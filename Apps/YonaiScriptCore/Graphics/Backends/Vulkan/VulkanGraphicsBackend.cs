using Yonai.IO;
using Yonai.Graphics.RenderPaths;
using System.Runtime.CompilerServices;
using System;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsBackend : IGraphicsBackend
	{
		public IGraphicsDevice Device => SelectedDevice;

		public VulkanInstance Instance { get; private set; }

		public string[] AvailableExtensions { get; private set; }

		public const int MaxFramesInFlight = 2;
		public VulkanSwapchain Swapchain { get; private set; } = null;
		public VulkanDevice SelectedDevice { get; private set; } = null;
		public VulkanCommandPool CommandPool { get; private set; } = null;

		public IRenderPath RenderPath { get; set; }

		public VkSampleCount MSAASamples { get; private set; } = VkSampleCount._1;

		private bool m_FramebufferResized = false;
		private FunctionQueue DeletionQueue = new FunctionQueue();

		#region Immediate Mode Submission
		private VulkanFence m_ImmediateFence;
		private VulkanCommandBuffer m_ImmediateCmdBuffer;
		private VulkanCommandPool m_ImmediateCmdPool;
		#endregion

		public Model TestModel { get; private set; }
		public Texture TestModelTexture { get; private set; }

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
			DeletionQueue.Enqueue(() => Instance.Dispose());

			AvailableExtensions = _GetAvailableExtensions();
			Log.Debug($"Available extensions: ({AvailableExtensions.Length})");
			foreach (string extension in AvailableExtensions)
				Log.Debug($" - {extension}");

			SelectedDevice = Instance.Devices[0];
			MSAASamples = SelectedDevice.Limits.GetMaxSamples();
			Log.Debug("\n\nChoosing first device - " + SelectedDevice.Name);

			Swapchain = SelectedDevice.CreateSwapchain();
			DeletionQueue.Enqueue(() => Swapchain.Dispose());

			CommandPool = new VulkanCommandPool(SelectedDevice);
			DeletionQueue.Enqueue(() => CommandPool.Dispose());

			CreateTestModel();

			#region Frame data
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
				m_Frames[i].MainCommandBuffer = m_Frames[i].CommandPool.CreateCommandBuffer();
			}

			DeletionQueue.Enqueue(() =>
			{
				for(int i = 0; i < m_Frames.Length; i++)
				{
					m_Frames[i].RenderSemaphore.Dispose();
					m_Frames[i].SwapchainSemaphore.Dispose();
					m_Frames[i].RenderFence.Dispose();
					m_Frames[i].CommandPool.Dispose();
				}
			});
			#endregion

			#region Render Path
			if (RenderPath == null)
			{
				RenderPath = new ForwardRenderPath();
				DeletionQueue.Enqueue(() => RenderPath.Dispose());
			}
			#endregion

			#region Immediate submission resources
			m_ImmediateCmdPool = new VulkanCommandPool(SelectedDevice);
			m_ImmediateCmdBuffer = m_ImmediateCmdPool.CreateCommandBuffer();
			DeletionQueue.Enqueue(() => m_ImmediateCmdPool.Dispose());

			m_ImmediateFence = new VulkanFence(SelectedDevice);
			DeletionQueue.Enqueue(() => m_ImmediateFence.Dispose());
			#endregion

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

		public void ImmediateSubmit(Action<VulkanCommandBuffer> callback)
		{
			m_ImmediateFence.Reset();
			VulkanCommandBuffer cmd = m_ImmediateCmdBuffer;

			cmd.Reset();
			cmd.Begin(VkCommandBufferUsage.OneTimeSubmit);

			callback?.Invoke(cmd);

			cmd.End();
			
			SelectedDevice.GraphicsQueue.Submit(
				buffers: new VulkanCommandBuffer[] { cmd },
				fence: m_ImmediateFence,
				waitSemaphores: null,
				signalSemaphores: null,
				stageMask: null
			);

			m_ImmediateFence.Wait();
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
			TestModelTexture = Resource.Load<Texture>("app://TestModel/viking_room.png");
		}

		public void Destroy()
		{
			Log.Trace("Destroying vulkan graphics backend");

			DeletionQueue.Flush();
			SelectedDevice = null;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern string[] _GetAvailableExtensions();
	}
}