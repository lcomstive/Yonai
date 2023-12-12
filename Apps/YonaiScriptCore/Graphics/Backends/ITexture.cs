using System;

namespace Yonai.Graphics
{
	public interface ITexture : IDisposable
	{
		IVector2 Resolution { get; }

		void GenerateMipmaps();
		void Upload(byte[] data);
	}
}