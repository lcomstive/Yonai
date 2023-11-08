using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanDevice : IDisposable
	{
		private struct QueueFamilyIndices
		{
			public uint? GraphicsFamily;
			public uint? PresentFamily;

			public bool IsComplete => GraphicsFamily != null && PresentFamily != 0;
		}

		private VulkanInstance m_Owner;
		internal IntPtr Device = IntPtr.Zero;
		internal IntPtr PhysicalDevice = IntPtr.Zero;
		internal IntPtr GraphicsQueue = IntPtr.Zero;
		internal IntPtr PresentQueue = IntPtr.Zero;

		public uint ID { get; private set; }
		public string Name { get; private set; }
		public uint DriverVersion { get; private set; }
		public PhysicalDeviceType Type { get; private set; }

		internal VulkanDevice(VulkanInstance owner, IntPtr physicalDeviceHandle)
		{
			m_Owner = owner;
			PhysicalDevice = physicalDeviceHandle;

			QueueFamilyIndices indices = FindQueueFamilies();
			Device = _CreateDevice(PhysicalDevice, indices.GraphicsFamily.Value, indices.PresentFamily.Value);
			GraphicsQueue = _GetDeviceQueue(Device, indices.GraphicsFamily.Value, 0);
			PresentQueue = _GetDeviceQueue(Device, indices.PresentFamily.Value, 0);

			Name = _GetPhysicalDeviceProperties(
				PhysicalDevice,
				out uint id,
				out uint driverVersion,
				out int deviceType
				);
			ID = id;
			DriverVersion = driverVersion;
			Type = (PhysicalDeviceType)deviceType;

			Log.Debug($"Device: {Name} [{ID}][Driver {DriverVersion}][{Enum.GetName(typeof(PhysicalDeviceType), Type)}]");
		}

		public void Dispose() => _DestroyDevice(Device);

		private QueueFamilyIndices FindQueueFamilies()
		{
			QueueFamilyIndices indices = new QueueFamilyIndices();

			int[] queueFamilies = _GetQueueFamilyProperties(PhysicalDevice);

			for(uint i = 0; i < queueFamilies.Length; i++)
			{
				QueueFlags flags = (QueueFlags)queueFamilies[i];
				if (flags.HasFlag(QueueFlags.Graphics))
					indices.GraphicsFamily = i;

				bool presentSupport = GetSurfaceSupport(i);
				if(presentSupport)
					indices.PresentFamily = i;

				if (indices.IsComplete)
					break;
			}

			return indices;
		}

		public bool GetSurfaceSupport(uint queueFamilyIndex) => _GetPhysicalDeviceSurfaceSupport(PhysicalDevice, queueFamilyIndex, m_Owner.m_Surface);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _CreateDevice(IntPtr physicalDevice, uint graphicsFamilyIndice, uint presentFamilyIndice);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroyDevice(IntPtr logicalDevice);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int[] _GetQueueFamilyProperties(IntPtr physicalDevice);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetDeviceQueue(IntPtr logicalDevice, uint queueFamilyIndex, uint queueIndex);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetPhysicalDeviceSurfaceSupport(IntPtr physicalDevice, uint queueFamilyIndex, IntPtr surface);
		
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPhysicalDeviceProperties(
			IntPtr physicalDevice,
			out uint id,
			out uint driverVersion,
			out int deviceType
		);
	}
}