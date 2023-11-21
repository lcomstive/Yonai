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

		public Extents(IVector2 size)
		{
			Width  = (uint)size.x;
			Height = (uint)size.y;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct Extents3D
	{
		public uint Width, Height, Depth;

		public Extents3D(uint width, uint height, uint depth = 1)
		{
			Width = width;
			Height = height;
			Depth = depth;
		}

		public Extents3D(float width, float height, float depth = 1)
		{
			Width = (uint)width;
			Height = (uint)height;
			Depth = (uint)depth;
		}

		public Extents3D(int width, int height, int depth = 1)
		{
			Width = (uint)width;
			Height = (uint)height;
			Depth = (uint)depth;
		}

		public Extents3D(IVector3 size)
		{
			Width = (uint)size.x;
			Height = (uint)size.y;
			Depth = (uint)size.z;
		}

		public Extents3D(IVector2 size, int depth = 1)
		{
			Width = (uint)size.x;
			Height = (uint)size.y;
			Depth = (uint)depth;
		}

		public static implicit operator IVector2(Extents3D extents) => new IVector2((int)extents.Width, (int)extents.Height);
		public static implicit operator IVector3(Extents3D extents) => new IVector3((int)extents.Width, (int)extents.Height, (int)extents.Depth);
	}
}