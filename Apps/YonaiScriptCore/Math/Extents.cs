using System.Runtime.InteropServices;

namespace Yonai
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Extents
	{
		public uint Width, Height;

		public Extents(uint width, uint height)
		{
			Width = width;
			Height = height;
		}

		public Extents(float width, float height)
		{
			Width = (uint)width;
			Height = (uint)height;
		}

		public Extents(int width, int height)
		{
			Width = (uint)width;
			Height = (uint)height;
		}
	}
}