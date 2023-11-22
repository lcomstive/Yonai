using System;

namespace Yonai.Graphics
{
	public interface IBuffer : IDisposable
	{
		void Upload(byte[] data);
	}
}