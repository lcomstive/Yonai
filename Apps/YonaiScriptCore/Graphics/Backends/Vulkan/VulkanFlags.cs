using System;

namespace Yonai.Graphics.Backends.Vulkan
{
	[Flags]
	public enum QueueFlags : int
	{
		Graphics		= 0x00000001,
		Compute			= 0x00000002,
		Transfer		= 0x00000004,
		SparseBinding	= 0x00000008,
		Protected		= 0x00000010,
		VideoDecodeKHR	= 0x00000020,
		OpticalFlowNV	= 0x00000100
	}

	public enum PhysicalDeviceType : int
	{
		Other			= 0,
		IntegratedGPU	= 1,
		DiscreteGPU		= 2,
		VirtualGPU		= 3,
		CPU				= 4
	}
}