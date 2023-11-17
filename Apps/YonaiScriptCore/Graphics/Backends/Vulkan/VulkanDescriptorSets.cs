using System;
using Yonai._Internal;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanDescriptorSetLayout : IDisposable
	{
		internal IntPtr Handle;
		private VulkanDevice m_Device;

		internal VulkanDescriptorSetLayout(VulkanDevice device, VkDescriptorSetLayoutBinding[] bindings)
		{
			m_Device = device;
			IntPtr bindingsPtr = InteropUtils.CreateNativeHandle(bindings);
			VkResult result = (VkResult)_Create(m_Device.Device, bindingsPtr, (uint)bindings.Length, out Handle);
			if(result != VkResult.Success)
				Log.Error("Failed to create descriptor set layout");
		}

		public void Dispose() => _Destroy(m_Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(IntPtr device, IntPtr layoutBindingsArr, uint layoutBindingsCount, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);
	}

	public class VulkanDescriptorPool : IDisposable
	{
		internal IntPtr Handle;
		private VulkanDevice m_Device;

		internal VulkanDescriptorPool(VulkanDevice device, VkDescriptorType type, int descriptorCount, int maxDescriptorSets)
		{
			m_Device = device;
			VkResult result = (VkResult)_Create(device.Device, (int)type, (uint)descriptorCount, (uint)maxDescriptorSets, out Handle);
			if (result != VkResult.Success)
				Log.Error("Failed to create descriptor pool");
		}

		public void Dispose() => _Destroy(m_Device.Device, Handle);

		public VulkanDescriptorSet[] AllocateDescriptorSets(int count, VulkanDescriptorSetLayout layout, VulkanBuffer[] uniformBuffers, int bufferSize, VkDescriptorType type)
		{
			IntPtr[] nativeBuffers = new IntPtr[uniformBuffers.Length];
			for (int i = 0; i < nativeBuffers.Length; i++)
				nativeBuffers[i] = uniformBuffers[i].BufferHandle;

			VkResult result = (VkResult)_AllocateDescriptorSets(
				m_Device.Device,
				Handle,
				count,
				layout.Handle,
				nativeBuffers,
				(uint)bufferSize,
				(int)type,
				out IntPtr[] output
			);
			if (result != VkResult.Success)
				return new VulkanDescriptorSet[0];

			VulkanDescriptorSet[] sets = new VulkanDescriptorSet[output.Length];
			for(int i = 0; i < output.Length; i++)
				sets[i] = new VulkanDescriptorSet(output[i]);
			return sets;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(IntPtr device, int type, uint descriptorCount, uint maxDescriptorSets, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _AllocateDescriptorSets(
			IntPtr device,
			IntPtr handle,
			int count,
			IntPtr descriptorSetLayout,
			IntPtr[] uniformBuffers,
			uint bufferSize,
			int descriptorType,
			out IntPtr[] output
		);
	}

	public class VulkanDescriptorSet
	{
		internal IntPtr Handle;

		internal VulkanDescriptorSet(IntPtr handle) => Handle = handle;
	}
}

