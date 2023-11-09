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

		private VulkanInstance m_Instance;
		internal IntPtr Device = IntPtr.Zero;
		internal IntPtr PhysicalDevice = IntPtr.Zero;
		internal IntPtr GraphicsQueue = IntPtr.Zero;
		internal IntPtr PresentQueue = IntPtr.Zero;

		public uint ID { get; private set; }
		public string Name { get; private set; }
		public uint DriverVersion { get; private set; }
		public VkPhysicalDeviceType Type { get; private set; }

		internal VulkanDevice(VulkanInstance owner, IntPtr physicalDeviceHandle)
		{
			m_Instance = owner;
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
			Type = (VkPhysicalDeviceType)deviceType;

			Log.Debug($"Device: {Name} [{ID}][Driver {DriverVersion}][{Enum.GetName(typeof(VkPhysicalDeviceType), Type)}]");
		}

		public void Dispose() => _DestroyDevice(Device);

		private QueueFamilyIndices FindQueueFamilies()
		{
			QueueFamilyIndices indices = new QueueFamilyIndices();

			int[] queueFamilies = _GetQueueFamilyProperties(PhysicalDevice);

			for(uint i = 0; i < queueFamilies.Length; i++)
			{
				VkQueueFlags flags = (VkQueueFlags)queueFamilies[i];
				if (flags.HasFlag(VkQueueFlags.Graphics))
					indices.GraphicsFamily = i;

				bool presentSupport = GetSurfaceSupport(i);
				if(presentSupport)
					indices.PresentFamily = i;

				if (indices.IsComplete)
					break;
			}

			return indices;
		}

		public bool GetSurfaceSupport(uint queueFamilyIndex) => _GetPhysicalDeviceSurfaceSupport(PhysicalDevice, queueFamilyIndex, m_Instance.m_Surface);

		public VulkanSwapchain CreateSwapchain() => new VulkanSwapchain(this, m_Instance);
		
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