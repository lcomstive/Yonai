﻿namespace AquaEngine.Graphics
{
	public interface IRenderPipeline
	{
		IVector2 Resolution { get; set; }

		Framebuffer Output { get; }

		/// <summary>
		/// Draw the scene from <paramref name="camera"/>'s perspective
		/// </summary>
		void Draw(Camera camera);
	}
}
