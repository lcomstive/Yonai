using Newtonsoft.Json.Linq;
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Yonai._Internal;
using static System.Net.Mime.MediaTypeNames;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanCommandBuffer : IDisposable
	{
		public IntPtr Handle { get; private set; }
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

		public void DrawIndexed(uint indexCount, uint instanceCount = 1, uint firstIndex = 0, uint vertexOffset = 0, uint firstInstance = 0) =>
			_DrawIndexed(Handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);

		public void BindPipeline(VulkanPipeline pipeline, VkPipelineBindPoint bindPoint) =>
			_BindPipeline(Handle, (int)bindPoint, pipeline.Handle);

		public void SetViewport(VkViewport viewport, int index = 0) =>
			_SetViewport(Handle, ref viewport, index);

		public void SetScissor(VkRect2D scissor, int index = 0) =>
			_SetScissor(Handle, ref scissor, index);

		public void BeginRenderPass(VulkanRenderPass renderPass, VulkanFramebuffer framebuffer, IVector2 offset, IVector2 extent, Colour clearValue) =>
			BeginRenderPass(renderPass, framebuffer, offset, extent, new VkClearValue[] { new VkClearValue(clearValue) });

		public void BeginRenderPass(VulkanRenderPass renderPass, VulkanFramebuffer framebuffer, IVector2 offset, IVector2 extent, VkClearValue[] clearValues)
		{
			VkClearValueNative[] nativeClears = new VkClearValueNative[clearValues.Length];
			for (int i = 0; i < clearValues.Length; i++)
				nativeClears[i] = new VkClearValueNative(clearValues[i]);

			_BeginRenderPass(Handle, renderPass.Handle, framebuffer.Handle, ref offset, ref extent, nativeClears);
		}

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

		public void BindDescriptorSets(VulkanPipeline pipeline, VkPipelineBindPoint bindPoint, VulkanDescriptorSet descriptorSet) =>
			BindDescriptorSets(pipeline, bindPoint, 0, new VulkanDescriptorSet[] { descriptorSet });

		public void BindDescriptorSets(VulkanPipeline pipeline, VkPipelineBindPoint bindPoint, uint firstSet, VulkanDescriptorSet[] descriptorSets)
		{
			IntPtr[] nativeSets = new IntPtr[descriptorSets.Length];
			for (int i = 0; i < nativeSets.Length; i++)
				nativeSets[i] = descriptorSets[i].Handle;

			_BindDescriptorSets(
				Handle,
				(int)bindPoint,
				pipeline.PipelineLayout,
				firstSet,
				nativeSets
			);
		}

		public void PipelineBarrier(VkImageMemoryBarrier barrier, VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage) =>
			_PipelineBarrierImage(Handle, ref barrier, (int)sourceStage, (int)destinationStage);

		public void CopyBufferToImage(VulkanBuffer buffer, VulkanImage image, VkBufferImageCopy region) =>
			_CopyBufferToImage(Handle, ref region, buffer.BufferHandle, image.Image);

		public void TransitionImageLayout(VulkanImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			VkImageMemoryBarrier barrier = new VkImageMemoryBarrier
			{
				OldLayout = oldLayout,
				NewLayout = newLayout,

				SrcQueueFamilyIndex = VulkanQueue.Ignored,
				DstQueueFamilyIndex = VulkanQueue.Ignored,

				Image = image,

				SubresourceRange = new VkImageSubresourceRange
				{
					AspectMask = VkImageAspectFlags.Color,
					BaseMipLevel = 0,
					LevelCount = image.MipLevels,
					BaseArrayLayer = 0,
					LayerCount = 1,
				}
			};

			VkPipelineStageFlags sourceStage = VkPipelineStageFlags.None;
			VkPipelineStageFlags destinationStage = VkPipelineStageFlags.None;
			if (oldLayout == VkImageLayout.Undefined && newLayout == VkImageLayout.TRANSFER_DST_OPTIMAL)
			{
				barrier.SrcAccessMask = 0;
				barrier.DstAccessMask = VkAccessFlags.TransferWrite;

				sourceStage = VkPipelineStageFlags.TopOfPipe;
				destinationStage = VkPipelineStageFlags.Transfer;
			}
			else if (oldLayout == VkImageLayout.TRANSFER_DST_OPTIMAL && newLayout == VkImageLayout.SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.SrcAccessMask = VkAccessFlags.TransferWrite;
				barrier.DstAccessMask = VkAccessFlags.ShaderRead;

				sourceStage = VkPipelineStageFlags.Transfer;
				destinationStage = VkPipelineStageFlags.FragmentShader;
			}

			PipelineBarrier(barrier, sourceStage, destinationStage);
		}

		public void ClearColorImage(VulkanImage image, VkImageLayout imageLayout, Colour colour, VkImageSubresourceRange range) =>
			ClearColorImage(image, imageLayout, colour, new VkImageSubresourceRange[] { range });

		public void ClearColorImage(VulkanImage image, VkImageLayout imageLayout, Colour colour, VkImageSubresourceRange[] ranges) =>
			_ClearColorImage(Handle, image.Image, (int)imageLayout, ref colour, ranges);

		public void BlitImage(
			VulkanImage srcImage, VkImageLayout srcLayout,
			VulkanImage dstImage, VkImageLayout dstLayout,
			VkImageBlit[] regions, VkFilter filter) =>
			_BlitImage(Handle, srcImage.Image, (int)srcLayout, dstImage.Image, (int)dstLayout, regions, filter);

		public void CopyImageTo(
			VulkanImage srcImage, VkImageLayout srcLayout,
			VulkanImage dstImage, VkImageLayout dstLayout,
			VkFilter filter = VkFilter.Linear,
			VkImageAspectFlags aspect = VkImageAspectFlags.Color)
		{
			VkImageBlit blit = new VkImageBlit
			{
				SrcOffsets = new IVector3[] { IVector3.Zero, new IVector3(srcImage.Resolution, 1) },
				SrcSubresource = new VkImageSubresourceLayers
				{
					AspectMask = aspect,
					MipLevel = 0,
					BaseArrayLayer = 0,
					LayerCount = 1
				},

				DstOffsets = new IVector3[] { IVector3.Zero, new IVector3(dstImage.Resolution, 1) },
				DstSubresource = new VkImageSubresourceLayers
				{
					AspectMask = aspect,
					MipLevel = 0,
					BaseArrayLayer = 0,
					LayerCount = 1
				}
			};

			BlitImage(
				srcImage,
				srcLayout,
				dstImage,
				dstLayout,
				new VkImageBlit[] { blit },
				filter
			);
		}

		public void Dispatch(uint groupCountX, uint groupCountY, uint groupCountZ) =>
			_Dispatch(Handle, groupCountX, groupCountY, groupCountZ);

		public void BeginRendering(VkRenderingInfo info)
		{
			VkRenderingAttachmentInfoNative[] colourAttachments = new VkRenderingAttachmentInfoNative[info.ColorAttachments.Length];
			for(int i = 0; i < colourAttachments.Length; i++)
				colourAttachments[i] = new VkRenderingAttachmentInfoNative(info.ColorAttachments[i]);

			VkRenderingAttachmentInfoNative depthNative = new VkRenderingAttachmentInfoNative();
			VkRenderingAttachmentInfoNative stencilNative = new VkRenderingAttachmentInfoNative();
			IntPtr depthAttachment = IntPtr.Zero, stencilAttachment = IntPtr.Zero;

			if (info.DepthAttachment.HasValue)
			{
				depthNative = new VkRenderingAttachmentInfoNative(info.DepthAttachment.Value);
				depthAttachment = InteropUtils.CreateNativeHandle(depthNative);
			}
			if (info.StencilAttachment.HasValue)
			{
				stencilNative = new VkRenderingAttachmentInfoNative(info.StencilAttachment.Value);
				stencilAttachment = InteropUtils.CreateNativeHandle(stencilNative);
			}

			_BeginRendering(
				Handle,
				ref info.RenderArea,
				info.LayerCount,
				info.ViewMask,
				colourAttachments,
				depthAttachment,
				stencilAttachment
			);

			// Handle cleanup
			depthNative.Dispose();
			stencilNative.Dispose();

			for (int i = 0; i < colourAttachments.Length; i++)
				colourAttachments[i].Dispose();
		}

		public void EndRendering() => _EndRendering(Handle);

		public void PushConstants<T>(VulkanPipeline pipeline, VkShaderStage stage, uint offset, uint size, T data) where T : struct
		{
			IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(T)));

			Marshal.StructureToPtr(data, ptr, false);
			PushConstants(pipeline, stage, offset, size, ptr);
			Marshal.FreeHGlobal(ptr);
		}

		public void PushConstants<T>(VulkanPipeline pipeline, VkShaderStage stage, uint offset, uint size, T[] data) where T : struct
		{
			IntPtr ptr = InteropUtils.CreateNativeHandle(data);
			PushConstants(pipeline, stage, offset, size, ptr);
			Marshal.FreeHGlobal(ptr);
		}


		public void PushConstants(VulkanPipeline pipeline, VkShaderStage stage, uint offset, uint size, IntPtr data) =>
			_PushConstants(Handle, pipeline.PipelineLayout, (int)stage, offset, size, data);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Reset(IntPtr handle, int flag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Destroy(IntPtr device, IntPtr pool, IntPtr cmdBuffer);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _Begin(IntPtr handle, int usage);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _End(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _BindPipeline(IntPtr handle, int bindPoint, IntPtr pipeline);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Draw(IntPtr handle, uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DrawIndexed(IntPtr handle, uint indexCount, uint instanceCount, uint firstIndex, uint vertexOffset, uint firstInstance);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetViewport(IntPtr handle, ref VkViewport viewport, int index);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetScissor(IntPtr handle, ref VkRect2D scissor, int index);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BeginRenderPass(
			IntPtr handle,
			IntPtr renderPass,
			IntPtr framebuffer,
			ref IVector2 offset,
			ref IVector2 extent,
			VkClearValueNative[] clearValues
		);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _EndRenderPass(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BindVertexBuffers(IntPtr handle, IntPtr[] buffers, int[] offsets);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BindIndexBuffer(IntPtr handle, IntPtr buffer, int offset, int indexType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CopyBuffer(IntPtr handle, IntPtr src, IntPtr dst, int srcOffset, int dstOffset, int size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BindDescriptorSets(
			IntPtr handle,
			int bindPoint,
			IntPtr pipelineLayout,
			uint firstSet,
			IntPtr[] descriptorSets
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PipelineBarrierImage(IntPtr handle, ref VkImageMemoryBarrier barrier, int srcStage, int dstStage);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CopyBufferToImage(IntPtr handle, ref VkBufferImageCopy region, IntPtr buffer, IntPtr image);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BlitImage(
			IntPtr handle,
			IntPtr srcImage, int srcLayout,
			IntPtr dstImage, int dstLayout,
			VkImageBlit[] regions, VkFilter filter
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _ClearColorImage(IntPtr handle, IntPtr image, int imageLayout, ref Colour colour, VkImageSubresourceRange[] ranges);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Dispatch(IntPtr handle, uint x, uint y, uint z);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BeginRendering(
			IntPtr handle,
			ref VkRect2D renderArea,
			uint layerCount,
			uint viewMask,
			VkRenderingAttachmentInfoNative[] colourAttachments,
			IntPtr depthAttachment,
			IntPtr stencilAttachment
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _EndRendering(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PushConstants(
			IntPtr handle,
			IntPtr pipelineLayout,
			int stageFlags,
			uint offset,
			uint size,
			IntPtr values
		);
		#endregion
	}
}
