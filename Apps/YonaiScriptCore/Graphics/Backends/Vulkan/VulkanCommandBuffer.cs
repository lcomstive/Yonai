using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanCommandBuffer : IDisposable
	{
		internal IntPtr Handle;
		private VulkanCommandPool m_Pool;

		internal VulkanCommandBuffer(IntPtr handle, VulkanCommandPool pool)
		{
			m_Pool = pool;
			Handle = handle;
		}

		public void Dispose() => _Destroy(m_Pool.Device.Device, m_Pool.Handle, Handle);

		public VkResult Reset(VkCommandBufferResetFlag flag = VkCommandBufferResetFlag.None) =>
			(VkResult)_Reset(Handle, (int)flag);

		public VkResult Begin(VkCommandBufferUsage usage = 0) => (VkResult)_Begin(Handle, (int)usage);
		public VkResult End() => (VkResult)_End(Handle);

		public void Draw(uint vertexCount, uint instanceCount = 1, uint firstVertex = 0, uint firstInstance = 0) =>
			_Draw(Handle, vertexCount, instanceCount, firstVertex, firstInstance);

		public void DrawIndexed(uint indexCount, uint instanceCount = 1, uint firstIndex = 0, int vertexOffset = 0, uint firstInstance = 0) =>
			_DrawIndexed(Handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);

		public void BindPipeline(VulkanGraphicsPipeline pipeline, VkPipelineBindPoint bindPoint) =>
			_BindPipeline(Handle, (int)bindPoint, pipeline.Handle);

		public void SetViewport(VkViewport viewport, int index = 0) =>
			_SetViewport(Handle, ref viewport, index);

		public void SetScissor(VkRect2D scissor, int index = 0) =>
			_SetScissor(Handle, ref scissor, index);

		public void BeginRenderPass(VulkanRenderPass renderPass, VulkanFramebuffer framebuffer, IVector2 offset, IVector2 extent, Colour clearValue) =>
			_BeginRenderPass(Handle, renderPass.Handle, framebuffer.Handle, ref offset, ref extent, ref clearValue);

		public void EndRenderPass() => _EndRenderPass(Handle);

		public void BindVertexBuffer(VulkanBuffer buffer, int offset = 0) =>
			_BindVertexBuffers(Handle, new IntPtr[] { buffer.BufferHandle }, new int[] { offset });

		public void BindVertexBuffers(VulkanBuffer[] buffers, int[] offsets)
		{
			IntPtr[] handles = new IntPtr[buffers.Length];
			for(int i = 0;i < handles.Length;i++)
				handles[i] = buffers[i].BufferHandle;
			_BindVertexBuffers(Handle, handles, offsets);
		}

		public void BindIndexBuffer(VulkanBuffer buffer, int offset = 0, VkIndexType indexType = VkIndexType.UINT32) =>
			_BindIndexBuffer(Handle, buffer.BufferHandle, offset, (int)indexType);

		public void CopyBuffer(VulkanBuffer src, VulkanBuffer dst, int srcOffset, int dstOffset, int size) =>
			_CopyBuffer(Handle, src.BufferHandle, dst.BufferHandle, srcOffset, dstOffset, size);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Reset(IntPtr handle, int flag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Destroy(IntPtr device, IntPtr pool, IntPtr cmdBuffer);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _Begin(IntPtr handle, int usage);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _End(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _BindPipeline(IntPtr handle, int bindPoint, IntPtr pipeline);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Draw(IntPtr handle, uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DrawIndexed(IntPtr handle, uint indexCount, uint instanceCount, uint firstIndex, int vertexOffset, uint firstInstance);

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

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BindVertexBuffers(IntPtr handle, IntPtr[] buffers, int[] offsets);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BindIndexBuffer(IntPtr handle, IntPtr buffer, int offset, int indexType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CopyBuffer(IntPtr handle, IntPtr src, IntPtr dst, int srcOffset, int dstOffset, int size);
		#endregion
	}
}
