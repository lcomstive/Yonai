using Yonai.Graphics;

namespace Yonai.Systems
{
	public class RenderSystem : YonaiSystem
	{
		public override bool IsGlobal => true;

		protected override void Update()
		{
			IVector2 resolution = Window.Resolution;
			if(Renderer.Pipeline.Resolution != resolution)
				Renderer.Pipeline.Resolution = resolution;
		}

		// Copy pipeline output to window
		protected override void PostDraw() =>
			Renderer.Pipeline?.Output.BlitTo(null);
	}
}