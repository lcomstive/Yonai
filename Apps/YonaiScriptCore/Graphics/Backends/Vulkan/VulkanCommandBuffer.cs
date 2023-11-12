using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanCommandBuffer
	{
		internal IntPtr Handle;

		internal VulkanCommandBuffer(IntPtr handle) => Handle = handle;

		public VkResult Reset(VkCommandBufferResetFlag flag = VkCommandBufferResetFlag.None) =>
			(VkResult)_Reset(Handle, (int)flag);

		public VkResult Begin() => (VkResult)_Begin(Handle);
		public VkResult End() => (VkResult)_End(Handle);

		public void Draw(uint vertexCount, uint instanceCount = 1, uint firstVertex = 0, uint firstInstance = 0) =>
			_Draw(Handle, vertexCount, instanceCount, firstVertex, firstInstance);

		public void BindPipeline(VulkanGraphicsPipeline pipeline, VkPipelineBindPoint bindPoint) =>
			_BindPipeline(Handle, (int)bindPoint, pipeline.Handle);

		public void SetViewport(VkViewport viewport, int index = 0) =>
			_SetViewport(Handle, ref viewport, index);

		public void SetScissor(VkRect2D scissor, int index = 0) =>
			_SetScissor(Handle, ref scissor, index);

		public void BeginRenderPass(VulkanRenderPass renderPass, VulkanFramebuffer framebuffer, IVector2 offset, IVector2 extent, Colour clearValue) =>
			_BeginRenderPass(Handle, renderPass.Handle, framebuffer.Handle, ref offset, ref extent, ref clearValue);

		public void EndRenderPass() => _EndRenderPass(Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Reset(IntPtr handle, int flag);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _Begin(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _End(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _BindPipeline(IntPtr handle, int bindPoint, IntPtr pipeline);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Draw(IntPtr handle, uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetViewport(IntPtr handle, ref VkViewport viewport, int index);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetScissor(IntPtr handle, ref VkRect2D scissor, int index);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BeginRenderPass(
			IntPtr handle,
			IntPtr renderPass,
			IntPtr framebuffer,
			ref IVector2 offset,
			ref IVector2 extent,
			ref Colour clearValue
		);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _EndRenderPass(IntPtr handle);
	}
}
