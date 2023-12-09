using System;

namespace Yonai.Graphics.Backends.Vulkan
{
	public abstract class VulkanPipeline : IPipeline
	{
		virtual internal IntPtr Handle { get; }
		virtual internal IntPtr PipelineLayout { get; }
	}
}