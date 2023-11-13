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

		public VulkanGraphicsPipeline(VkGraphicsPipelineCreateInfo info)
		{
			RenderPass = info.RenderPass;
			Subpass = info.Subpass;
			Upload(Subpass, info);
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