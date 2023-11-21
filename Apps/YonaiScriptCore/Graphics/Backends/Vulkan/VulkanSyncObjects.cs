using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanFence : IDisposable
	{
		internal IntPtr Handle;
		internal VulkanDevice Device;

		public VulkanFence(VulkanDevice device, VkFenceCreateFlags flags = VkFenceCreateFlags.None) =>
			Handle = _Create((Device = device).Device, (int)flags);

		public void Dispose() => _Destroy(Device.Device, Handle);

		public void Wait(uint timeout = uint.MaxValue) => Wait(new VulkanFence[] { this }, true, timeout);

		public void Reset() => Reset(new VulkanFence[] { this });

		public static VkResult Wait(VulkanFence[] fences, bool waitAll = true, uint timeout = uint.MaxValue)
		{
			if (fences.Length == 0) return VkResult.Incomplete;

			IntPtr[] handles = new IntPtr[fences.Length];
			for(int i = 0; i < handles.Length; i++)
				handles[i] = fences[i].Handle;

			VkResult result = (VkResult)_Wait(fences[0].Device.Device, handles, waitAll, timeout);
			result.CheckForSuccess("Fence wait");
			return result;
		}
		
		public static VkResult Reset(VulkanFence[] fences)
		{
			if (fences.Length == 0) return VkResult.Incomplete;

			IntPtr[] handles = new IntPtr[fences.Length];
			for(int i = 0; i < handles.Length; i++)
				handles[i] = fences[i].Handle;

			VkResult result = (VkResult)_Reset(fences[0].Device.Device, handles);
			result.CheckForSuccess("Reset fence");
			return result;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(IntPtr device, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Wait(IntPtr device, IntPtr[] handles, bool waitAll, uint timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _Reset(IntPtr device, IntPtr[] handles);
	}

	public class VulkanSemaphore : IDisposable
	{
		internal IntPtr Handle;
		internal VulkanDevice Device;

		public VulkanSemaphore(VulkanDevice device) =>
			Handle = _Create((Device = device).Device);

		public void Dispose() => _Destroy(Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _Create(IntPtr device);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Destroy(IntPtr device, IntPtr handle);
	}
}