using System;

namespace Yonai.Graphics
{
	public enum BufferUsage
	{
		NoCPU,
		CPUWrite,
		CPURead
	}

	[Flags]
	public enum BufferType
	{
		None		= 0,
		Vertex		= 1 << 0,
		Index		= 1 << 1,
		Uniform		= 1 << 2,
		Storage		= 1 << 3,
		Indirect	= 1 << 4,
	}

	public interface IGraphicsDevice : IDisposable
	{
		IBuffer CreateBuffer(int bufferSize, BufferUsage usage, BufferType typeFlags = BufferType.None);

		void CopyBuffer(IBuffer srcBuffer, IBuffer dstBuffer);

		IShaderModule CreateShaderModule(byte[] data);
	}
}