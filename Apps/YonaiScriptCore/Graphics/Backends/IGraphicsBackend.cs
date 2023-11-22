namespace Yonai.Graphics.Backends
{
	public interface IGraphicsBackend
	{
		IGraphicsDevice Device { get; }

		void Create();
		void Destroy();
		void Draw();
	}
}