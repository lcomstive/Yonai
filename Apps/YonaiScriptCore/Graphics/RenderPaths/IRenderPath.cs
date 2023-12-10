using System;
using Yonai.Graphics.Backends.Vulkan;

namespace Yonai.Graphics
{
	public interface IRenderPath : IDisposable
	{
		VulkanImage ColourOutput { get; }

		/// <summary>
		/// Draw the scene from <paramref name="camera"/>'s perspective
		/// </summary>
		void Draw(VulkanCommandBuffer commandBuffer, Camera camera);

		/// <summary>
		/// Called when window has been resized
		/// </summary>
		void OnResized(IVector2 resolution);
	}
}
