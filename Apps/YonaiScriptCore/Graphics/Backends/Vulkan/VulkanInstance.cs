using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanInstance : IDisposable
	{
		internal IntPtr m_Handle = IntPtr.Zero;
		internal IntPtr m_Surface = IntPtr.Zero;
		internal IntPtr m_DebugMessenger = IntPtr.Zero;

		public VulkanDevice[] Devices { get; private set; } = new VulkanDevice[0];

		public VulkanInstance(string appName, Version appVersion)
		{
			m_Handle = _Create(appName, appVersion.Major, appVersion.Minor, appVersion.Patch);
			m_DebugMessenger = _CreateDebugMessenger(m_Handle);

			m_Surface = _CreateSurface(m_Handle);

			GetPhysicalDevices();
		}

		public void Dispose()
		{
			foreach (VulkanDevice device in Devices)
				device.Dispose();
			Devices = new VulkanDevice[0];

			_DestroySurface(m_Handle, m_Surface);
			_DestroyDebugMessenger(m_Handle, m_DebugMessenger);
			_Destroy(m_Handle);
		}

		private void GetPhysicalDevices()
		{
			IntPtr[] deviceHandles = _GetPhysicalDevices(m_Handle);
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