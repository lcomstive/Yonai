using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanInstance : IDisposable
	{
		internal IntPtr m_Handle = IntPtr.Zero;
		internal IntPtr m_Surface = IntPtr.Zero;
		internal IntPtr m_DebugMessenger = IntPtr.Zero;

		public VulkanDevice[] Devices { get; private set; } = new VulkanDevice[0];

		public VulkanInstance(string appName, Version appVersion)
		{
			m_Handle = _Create(appName, appVersion.Major, appVersion.Minor, appVersion.Patch);
			m_DebugMessenger = _CreateDebugMessenger(m_Handle);

			m_Surface = _CreateSurface(m_Handle);

			GetPhysicalDevices();

			Log.Debug("\n\nChoosing first device");
			VulkanDevice device = Devices[0];
			VulkanSwapchain swapchain = device.CreateSwapchain();

			VkAttachmentDescription[] attachments = new VkAttachmentDescription[]
			{
				new VkAttachmentDescription()
				{
					Format = swapchain.ImageFormat,
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
			VulkanRenderPass renderPass = new VulkanRenderPass(device, attachments, subpasses, dependencies);
		}

		public void Dispose()
		{
			_DestroySurface(m_Handle, m_Surface);
			_DestroyDebugMessenger(m_Handle, m_DebugMessenger);
			_Destroy(m_Handle);
		}

		private void GetPhysicalDevices()
		{
			IntPtr[] deviceHandles = _GetPhysicalDevices(m_Handle);
			Devices = new VulkanDevice[deviceHandles.Length];
			for(int i = 0; i <  deviceHandles.Length; i++)
				Devices[i] = new VulkanDevice(this, deviceHandles[i]);
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _Create(
			string appName,
			int versionMajor,
			int versionMinor,
			int versionPatch
		);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Destroy(IntPtr instance);
		
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _CreateDebugMessenger(IntPtr instance);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroyDebugMessenger(IntPtr instance, IntPtr messenger);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _CreateSurface(IntPtr instance);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroySurface(IntPtr instance, IntPtr surface);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr[] _GetPhysicalDevices(IntPtr instance);
	}
}