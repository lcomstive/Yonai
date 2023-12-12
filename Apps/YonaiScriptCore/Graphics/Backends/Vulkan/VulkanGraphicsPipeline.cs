using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsPipeline : VulkanPipeline, IDisposable
	{
		private IntPtr m_Handle;
		internal override IntPtr Handle => m_Handle;

		private IntPtr m_PipelineLayout;
		internal override IntPtr PipelineLayout => m_PipelineLayout;

		internal VulkanRenderPass RenderPass;
		private VulkanDevice m_Device;

		public VkGraphicsPipelineCreateInfo PipelineCreateInfo { get; private set; }
		public uint Subpass { get; private set; }

		/// <summary>
		/// Creates a graphics pipeline object, using the <see cref="VulkanDevice"/>
		/// that created <see cref="VkGraphicsPipelineCreateInfo.RenderPass"/>
		/// </summary>
		public VulkanGraphicsPipeline(VkGraphicsPipelineCreateInfo info) :
			this(info.RenderPass?.Device, info) {}

		/// <summary>
		/// Creates a graphics pipeline object, with a manually specified <see cref="VulkanDevice"/>
		/// </summary>
		public VulkanGraphicsPipeline(VulkanDevice device, VkGraphicsPipelineCreateInfo info)
		{
			RenderPass = info.RenderPass;
			Subpass = info.Subpass;
			Upload(Subpass, info);
		}

		public void Dispose() => _Destroy(Handle, PipelineLayout);

		private void Upload(uint subpass, VkGraphicsPipelineCreateInfo pipelineInfo)
		{
			Subpass = subpass;
			PipelineCreateInfo = pipelineInfo;

			VkGraphicsPipelineCreateInfoNative native = new VkGraphicsPipelineCreateInfoNative(RenderPass, Subpass, PipelineCreateInfo);
			m_Handle = _Create(m_Device.Device, ref native, out m_PipelineLayout);
			native.Dispose();
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(IntPtr device, ref VkGraphicsPipelineCreateInfoNative pipelineInfo, out IntPtr pipelineLayout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr handle, IntPtr pipelineLayout);
	}
}