using System;
using Yonai.Graphics.Backends.Vulkan;

namespace Yonai.Graphics
{
	public interface IRenderPath : IDisposable
	{
		VulkanImage ColourOutput { get; }

		/// <summary>
		/// Draw the scene into <see cref="ColourOutput"/>
		/// </summary>
		void Draw(VulkanCommandBuffer commandBuffer);

		/// <summary>
		/// Called when window has been resized
		/// </summary>
		void OnResized(IVector2 resolution);
	}
}
