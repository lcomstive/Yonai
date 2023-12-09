using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanComputePipeline : VulkanPipeline, IDisposable
	{
		private IntPtr m_Handle;
		internal override IntPtr Handle => m_Handle;

		private IntPtr m_PipelineLayout;
		internal override IntPtr PipelineLayout => m_PipelineLayout;

		private VulkanDevice m_Device;

		public VulkanComputePipeline(VulkanDevice device, VkComputePipelineCreateInfo createInfo)
		{
			m_Device = device;

			VkComputePipelineCreateInfoNative nativeInfo = new VkComputePipelineCreateInfoNative(createInfo);
			m_Handle = _Create(m_Device.Device, ref nativeInfo, out m_PipelineLayout);
			nativeInfo.Dispose();
		}

		public void Dispose() => _Destroy(Handle, PipelineLayout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(IntPtr device, ref VkComputePipelineCreateInfoNative pipelineInfo, out IntPtr pipelineLayout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr handle, IntPtr pipelineLayout);
	}
}