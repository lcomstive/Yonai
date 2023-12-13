using System;
using Yonai.Systems;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanDevice : IGraphicsDevice
	{
		private struct QueueFamilyIndices
		{
			public uint? GraphicsFamily;
			public uint? PresentFamily;

			public bool IsComplete => GraphicsFamily != null && PresentFamily != 0;
		}

		public IntPtr Device { get; private set; } = IntPtr.Zero;
		public IntPtr PhysicalDevice { get; private set; } = IntPtr.Zero;
		public VulkanQueue GraphicsQueue { get; private set; } = null;
		public VulkanQueue PresentQueue { get; private set; } = null;
		public IntPtr Allocator { get; private set; } = IntPtr.Zero;

		private VulkanInstance m_Instance;
		internal VulkanGraphicsBackend m_Backend;

		public uint ID { get; private set; }
		public string Name { get; private set; }
		public uint DriverVersion { get; private set; }
		public VkPhysicalDeviceType Type { get; private set; }
		public VkPhysicalDeviceLimits Limits { get; private set; }

		internal VulkanDevice(VulkanInstance owner, IntPtr physicalDeviceHandle)
		{
			m_Instance = owner;
			PhysicalDevice = physicalDeviceHandle;
			m_Backend = (VulkanGraphicsBackend)RenderSystem.Backend;

			QueueFamilyIndices indices = FindQueueFamilies();
			Device = _CreateDevice(PhysicalDevice, indices.GraphicsFamily.Value, indices.PresentFamily.Value);
			GraphicsQueue = new VulkanQueue(_GetDeviceQueue(Device, indices.GraphicsFamily.Value, 0), indices.GraphicsFamily.Value);
			PresentQueue = new VulkanQueue(_GetDeviceQueue(Device, indices.PresentFamily.Value, 0), indices.PresentFamily.Value);

			Name = _GetPhysicalDeviceProperties(
				PhysicalDevice,
				out uint id,
				out uint driverVersion,
				out int deviceType
				);
			ID = id;
			DriverVersion = driverVersion;
			Type = (VkPhysicalDeviceType)deviceType;

			_GetPhysicalDeviceLimits(PhysicalDevice, out VkPhysicalDeviceLimits limits);
			Limits = limits;

			Allocator = _CreateAllocator(PhysicalDevice, Device, m_Instance.Handle);

			Log.Debug($"Device: {Name} [{ID}][Driver {DriverVersion}][{Enum.GetName(typeof(VkPhysicalDeviceType), Type)}]");
		}

		#region Testing Interface
		public IBuffer CreateBuffer(int bufferSize, BufferUsage usage, BufferType type)
		{
			VkMemoryProperty vkMemory = 0;
			switch(usage)
			{
				default:
				case BufferUsage.NoCPU: vkMemory |= VkMemoryProperty.DeviceLocal; break;
				case BufferUsage.CPURead:
				case BufferUsage.CPUWrite: vkMemory |= VkMemoryProperty.HostVisible | VkMemoryProperty.HostCoherent; break;
			}

			VkBufferUsage vkUsage = VkBufferUsage.TransferSource | VkBufferUsage.TransferDestination;
			if (type.HasFlag(BufferType.Vertex)) vkUsage |= VkBufferUsage.Vertex;
			if (type.HasFlag(BufferType.Index)) vkUsage |= VkBufferUsage.Index;
			if (type.HasFlag(BufferType.Uniform)) vkUsage |= VkBufferUsage.Uniform;
			if (type.HasFlag(BufferType.Indirect)) vkUsage |= VkBufferUsage.Indirect;

			return new VulkanBuffer(this, bufferSize, vkUsage, vkMemory);
		}

		public void CopyBuffer(IBuffer srcBuffer, IBuffer dstBuffer)
		{
			VulkanBuffer source = (VulkanBuffer)srcBuffer;
			source.CopyTo(m_Backend.CommandPool, (VulkanBuffer)dstBuffer, source.BufferSize);
		}

		public IShaderModule CreateShaderModule(byte[] data) =>
			new VulkanShaderModule(this, data);

		public ITexture CreateTexture(byte[] data, VkFormat format, IVector2 resolution, float maxAnisotropy = 1.0f, bool generateMipmaps = true)
		{
			VulkanCommandPool cmdPool = m_Backend.CommandPool;

			VulkanImage image = (VulkanImage)CreateTexture(format, resolution, maxAnisotropy, generateMipmaps);

			image.Upload(data, cmdPool, generateMipmaps ? VkImageLayout.TRANSFER_DST_OPTIMAL : VkImageLayout.SHADER_READ_ONLY_OPTIMAL);

			if (generateMipmaps)
				image.GenerateMipmaps(cmdPool);

			return image;
		}

		public ITexture CreateTexture(VkFormat format, IVector2 resolution, float maxAnisotropy = 1.0f, bool generateMipmaps = true)
		{
			VkImageCreateInfo imageInfo = VkImageCreateInfo.Default;
			imageInfo.Format = format;
			imageInfo.Usage = VkImageUsage.TransferSrc | VkImageUsage.TransferDst | VkImageUsage.Sampled;
			imageInfo.Extent = new Extents3D(resolution);
			imageInfo.MipLevels = generateMipmaps ? VulkanImage.CalculateMipLevels(resolution) : 1;

			VkImageViewCreateInfo imageViewInfo = VkImageViewCreateInfo.Default;
			imageViewInfo.SubresourceRange.LevelCount = imageInfo.MipLevels;
			imageViewInfo.Format = imageInfo.Format;

			VkSamplerCreateInfo samplerInfo = VkSamplerCreateInfo.Default;
			samplerInfo.AnisotropyEnable = maxAnisotropy > 1.0f;
			samplerInfo.MaxAnisotropy = maxAnisotropy;
			samplerInfo.MaxLod = imageInfo.MipLevels;

			return new VulkanImage(this, imageInfo, imageViewInfo, samplerInfo);
		}
		#endregion

		public void Dispose()
		{
			_DestroyAllocator(Allocator);
			_DestroyDevice(Device);
		}

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

		public bool GetSurfaceSupport(uint queueFamilyIndex) => _GetPhysicalDeviceSurfaceSupport(PhysicalDevice, queueFamilyIndex, m_Instance.Surface);

		public void WaitIdle() => _WaitIdle(Device);

		public VulkanSwapchain CreateSwapchain() => new VulkanSwapchain(this, m_Instance);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _CreateDevice(IntPtr physicalDevice, uint graphicsFamilyIndice, uint presentFamilyIndice);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroyDevice(IntPtr logicalDevice);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _WaitIdle(IntPtr logicalDevice);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int[] _GetQueueFamilyProperties(IntPtr physicalDevice);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetDeviceQueue(IntPtr logicalDevice, uint queueFamilyIndex, uint queueIndex);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetPhysicalDeviceSurfaceSupport(IntPtr physicalDevice, uint queueFamilyIndex, IntPtr surface);
		
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPhysicalDeviceProperties(
			IntPtr physicalDevice,
			out uint id,
			out uint driverVersion,
			out int deviceType
		);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetPhysicalDeviceLimits(IntPtr physicalDevice, out VkPhysicalDeviceLimits output);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _CreateAllocator(IntPtr physicalDevice, IntPtr logicalDevice, IntPtr instance);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DestroyAllocator(IntPtr allocator);
	}
}