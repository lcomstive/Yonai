using System;
using Yonai._Internal;
using System.Runtime.CompilerServices;
using System.Collections.Generic;

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
			result.CheckForSuccess("Descriptor set layout creation");
		}

		public void Dispose() => _Destroy(m_Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(IntPtr device, IntPtr layoutBindingsArr, uint layoutBindingsCount, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);
	}

	public class VulkanDescriptorPool : IDisposable
	{
		public IntPtr Handle { get; private set; }
		private VulkanDevice m_Device;

		public VulkanDescriptorPool(VulkanDevice device, VkDescriptorPoolSize[] poolSizes, int maxDescriptorSets) :
			this(device, poolSizes, (uint)maxDescriptorSets) { }
		
		public VulkanDescriptorPool(VulkanDevice device, VkDescriptorPoolSize[] poolSizes, uint maxDescriptorSets)
		{
			m_Device = device;
			VkResult result = (VkResult)_Create(device.Device, poolSizes, maxDescriptorSets, out IntPtr handle);
			Handle = handle;
			result.CheckForSuccess("Descriptor pool creation");
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
			result.CheckForSuccess("Allocating descriptor sets");
			if (result != VkResult.Success)
				return new VulkanDescriptorSet[0];

			VulkanDescriptorSet[] sets = new VulkanDescriptorSet[output.Length];
			for(int i = 0; i < output.Length; i++)
				sets[i] = new VulkanDescriptorSet(m_Device, output[i]);
			return sets;
		}

		public VulkanDescriptorSet AllocateDescriptorSet(VulkanDescriptorSetLayout layout) =>
			AllocateDescriptorSets(1, layout)?[0];

		public void Reset() => _Reset(m_Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(IntPtr device, VkDescriptorPoolSize[] poolSizes, uint maxDescriptorSets, out IntPtr handle);

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

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Reset(IntPtr device, IntPtr handle);
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
			_UpdateDescriptorSet(m_Device.Device, native);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _UpdateDescriptorSet(IntPtr device, VkWriteDescriptorSetNative[] descriptorSet);
	}

	#region Utils
	public class VulkanDescriptorSetLayoutBuilder
	{
		private List<VkDescriptorSetLayoutBinding> m_Bindings = new List<VkDescriptorSetLayoutBinding>();

		public VulkanDescriptorSetLayoutBuilder Add(uint binding, VkDescriptorType descriptorType, VkShaderStage stage)
		{
			m_Bindings.Add(new VkDescriptorSetLayoutBinding
			{
				Binding = binding,
				DescriptorType = descriptorType,
				DescriptorCount = 1,
				StageFlags = stage
			});

			return this;
		}

		public void Clear() => m_Bindings.Clear();

		public VulkanDescriptorSetLayout Build(VulkanDevice device) =>
			new VulkanDescriptorSetLayout(device, m_Bindings.ToArray());
	}

	public class VulkanDescriptorAllocator
	{
		public struct PoolSizeRatio
		{
			public VkDescriptorType Type;
			public float Ratio;
		}

		public VulkanDescriptorPool Pool { get; private set; }

		private VulkanDevice m_Device;

		public VulkanDescriptorAllocator(VulkanDevice device) => m_Device = device;

		public void CreatePool(uint maxSets, PoolSizeRatio[] poolRatios)
		{
			List<VkDescriptorPoolSize> poolSizes = new List<VkDescriptorPoolSize>();
			foreach (PoolSizeRatio ratio in poolRatios)
				poolSizes.Add(new VkDescriptorPoolSize
				{
					Type = ratio.Type,
					DescriptorCount = (uint)Math.Round(ratio.Ratio * maxSets)
				});

			Pool = new VulkanDescriptorPool(m_Device, poolSizes.ToArray(), maxSets);
		}

		public void ClearDescriptors() => Pool?.Reset();

		public void DestroyPool()
		{
			Pool?.Dispose();
			Pool = null;
		}

		public VulkanDescriptorSet Allocate(VulkanDescriptorSetLayout layout) =>
			Pool?.AllocateDescriptorSet(layout);
	}
	#endregion
}

