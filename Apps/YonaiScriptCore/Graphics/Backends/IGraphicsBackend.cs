namespace Yonai.Graphics.Backends
{
	public interface IGraphicsBackend
	{
		IGraphicsDevice Device { get; }
		IRenderPath RenderPath { get; set; }

		void Create();
		void Destroy();
		void Draw();
	}
}