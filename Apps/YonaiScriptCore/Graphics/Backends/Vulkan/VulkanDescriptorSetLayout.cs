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
			if(result != VkResult.VK_SUCCESS)
				Log.Error("Failed to create descriptor set layout");
		}

		public void Dispose() => _Destroy(m_Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Create(IntPtr device, IntPtr layoutBindingsArr, uint layoutBindingsCount, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);
	}
}

