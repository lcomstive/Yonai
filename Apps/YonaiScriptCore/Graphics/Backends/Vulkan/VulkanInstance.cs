using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanInstance : IDisposable
	{
		public IntPtr Handle { get; private set; } = IntPtr.Zero;
		public IntPtr Surface { get; private set; } = IntPtr.Zero;
		internal IntPtr m_DebugMessenger = IntPtr.Zero;

		public VulkanDevice[] Devices { get; private set; } = new VulkanDevice[0];

		public VulkanInstance(string appName, Version appVersion)
		{
			Handle = _Create(appName, appVersion.Major, appVersion.Minor, appVersion.Patch);
			m_DebugMessenger = _CreateDebugMessenger(Handle);

			Surface = _CreateSurface(Handle);

			GetPhysicalDevices();
		}

		public void Dispose()
		{
			foreach (VulkanDevice device in Devices)
				device.Dispose();
			Devices = new VulkanDevice[0];

			_DestroySurface(Handle, Surface);
			_DestroyDebugMessenger(Handle, m_DebugMessenger);
			_Destroy(Handle);
		}

		private void GetPhysicalDevices()
		{
			IntPtr[] deviceHandles = _GetPhysicalDevices(Handle);
			Devices = new VulkanDevice[deviceHandles.Length];
			for(int i = 0; i <  deviceHandles.Length; i++)
				Devices[i] = new VulkanDevice(this, deviceHandles[i]);
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _Create(
			string appName,
			int versionMajor,
			int versionMinor,
			int versionPatch
		);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Destroy(IntPtr instance);
		
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _CreateDebugMessenger(IntPtr instance);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroyDebugMessenger(IntPtr instance, IntPtr messenger);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _CreateSurface(IntPtr instance);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroySurface(IntPtr instance, IntPtr surface);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr[] _GetPhysicalDevices(IntPtr instance);
	}
}