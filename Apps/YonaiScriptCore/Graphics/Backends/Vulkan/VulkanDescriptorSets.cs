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

		internal VulkanDescriptorPool(VulkanDevice device, VkDescriptorPoolSize[] poolSizes, int descriptorCount, int maxDescriptorSets)
		{
			m_Device = device;
			VkResult result = (VkResult)_Create(device.Device, poolSizes, (uint)descriptorCount, (uint)maxDescriptorSets, out Handle);
			if (result != VkResult.Success)
				Log.Error("Failed to create descriptor pool");
		}

		public void Dispose() => _Destroy(m_Device.Device, Handle);

		public VulkanDescriptorSet[] AllocateDescriptorSets(int count, VulkanDescriptorSetLayout layout)
		{
			VkResult result = (VkResult)_AllocateDescriptorSets(
				m_Device.Device,
				Handle,
				count,
				layout.Handle,
				out IntPtr[] output
			);
			if (result != VkResult.Success)
				return new VulkanDescriptorSet[0];

			VulkanDescriptorSet[] sets = new VulkanDescriptorSet[output.Length];
			for(int i = 0; i < output.Length; i++)
				sets[i] = new VulkanDescriptorSet(m_Device, output[i]);
			return sets;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(IntPtr device, VkDescriptorPoolSize[] poolSizes, uint descriptorCount, uint maxDescriptorSets, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _AllocateDescriptorSets(
			IntPtr device,
			IntPtr handle,
			int count,
			IntPtr descriptorSetLayout,
			out IntPtr[] output
		);
	}

	public class VulkanDescriptorSet
	{
		internal IntPtr Handle;
		private VulkanDevice m_Device;

		internal VulkanDescriptorSet(VulkanDevice device, IntPtr handle)
		{
			m_Device = device;
			Handle = handle;
		}

		public void Update(VkWriteDescriptorSet[] sets)
		{
			VkWriteDescriptorSetNative[] native = new VkWriteDescriptorSetNative[sets.Length];
			for (int i = 0; i < sets.Length; i++)
				native[i] = new VkWriteDescriptorSetNative(Handle, sets[i]);
			_UpdateDescriptorSet(m_Device.Device, native, sets.Length);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _UpdateDescriptorSet(IntPtr device, VkWriteDescriptorSetNative[] descriptorSet, int descriptorSetCount);
	}
}

