using System;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("({x}, {y}, {z}, {w})")]
	public struct IVector4
	{
		public int x, y, z, w;

		public static IVector4 Zero => new IVector4(0, 0, 0, 0);
		public static IVector4 One	=> new IVector4(1, 1, 1, 1);

		public IVector4(int x = 0, int y = 0, int z = 0, int w = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
		}

		public static IVector4 Lerp(IVector4 a, IVector4 b, float time) =>
			new IVector4(
				(int)Math.Round(MathUtils.Lerp(a.x, b.x, time), 0),
				(int)Math.Round(MathUtils.Lerp(a.y, b.y, time), 0),
				(int)Math.Round(MathUtils.Lerp(a.z, b.z, time), 0),
				(int)Math.Round(MathUtils.Lerp(a.w, b.w, time), 0)
				);

		public override string ToString() => $"({x}, {y}, {z}, {w})";

		public static implicit operator string(IVector4 v) => v.ToString();

		public static IVector4 operator +(IVector4 a, IVector4 b) => new IVector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		public static IVector4 operator -(IVector4 a, IVector4 b) => new IVector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		public static IVector4 operator *(IVector4 a, IVector4 b) => new IVector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		public static IVector4 operator /(IVector4 a, IVector4 b) => new IVector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);

		public static IVector4 operator +(IVector4 a, IVector3 b) => new IVector4(a.x + b.x, a.y + b.y, a.z, a.w);
		public static IVector4 operator -(IVector4 a, IVector3 b) => new IVector4(a.x - b.x, a.y - b.y, a.z, a.w);
		public static IVector4 operator +(IVector3 b, IVector4 a) => new IVector4(a.x + b.x, a.y + b.y, a.z, a.w);
		public static IVector4 operator -(IVector3 b, IVector4 a) => new IVector4(a.x - b.x, a.y - b.y, a.z, a.w);

		public static IVector4 operator *(IVector4 a, int b) => new IVector4(a.x * b, a.y * b, a.z * b, a.w * b);
		public static IVector4 operator /(IVector4 a, int b) => new IVector4(a.x / b, a.y / b, a.z / b, a.w / b);

		public static Vector4 operator *(IVector4 a, float b) => new Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
		public static Vector4 operator /(IVector4 a, float b) => new Vector4(a.x / b, a.y / b, a.z / b, a.w / b);
	}
}
