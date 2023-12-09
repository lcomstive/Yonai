using System;
using Yonai.IO;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanShaderModule : IShaderModule, IDisposable
	{
		internal IntPtr Handle;
		internal VulkanDevice Device;

		public VulkanShaderModule(VulkanDevice device, byte[] data) =>
			Handle = _Create((Device = device).Device, data);

		public VulkanShaderModule(VulkanDevice device, VFSFile file)
		{
			Device = device;

			byte[] data = VFS.Read(file);
			if ((data?.Length ?? 0) > 0)
				Handle = _Create(Device.Device, data);
			else
				Log.Warning($"Unable to generate shader module from '{file}' - could not read file");
		}

		public void Dispose() => _Destroy(Device.Device, Handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern IntPtr _Create(IntPtr device, byte[] data);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Destroy(IntPtr device, IntPtr handle);

		public static implicit operator IntPtr(VulkanShaderModule module) => module.Handle;
	}
}