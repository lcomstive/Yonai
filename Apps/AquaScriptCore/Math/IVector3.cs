using System;
using AquaEngine.IO;
using Newtonsoft.Json.Linq;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("({x}, {y}, {z})")]
	public struct IVector3 : ISerializable
	{
		public int x, y, z;

		public static IVector3 Zero => new IVector3(0, 0, 0);
		public static IVector3 One => new IVector3(1, 1, 1);

		public IVector3(int x = 0, int y = 0, int z = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public static IVector3 Lerp(IVector3 a, IVector3 b, float time) =>
			new IVector3(
				(int)Math.Round(MathUtils.Lerp(a.x, b.x, time), 0),
				(int)Math.Round(MathUtils.Lerp(a.y, b.y, time), 0),
				(int)Math.Round(MathUtils.Lerp(a.z, b.z, time), 0)
				);

		public override string ToString() => $"({x}, {y}, {z})";

		public static implicit operator string(IVector3 v) => v.ToString();

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("x", x),
				new JProperty("y", y),
				new JProperty("z", z)
				);

		public void OnDeserialize(JObject json)
		{
			x = json["x"].Value<int>();
			y = json["y"].Value<int>();
			z = json["z"].Value<int>();
		}

		public static IVector3 operator +(IVector3 a, IVector3 b) => new IVector3(a.x + b.x, a.y + b.y, a.z + b.z);
		public static IVector3 operator -(IVector3 a, IVector3 b) => new IVector3(a.x - b.x, a.y - b.y, a.z - b.z);
		public static IVector3 operator *(IVector3 a, IVector3 b) => new IVector3(a.x * b.x, a.y * b.y, a.z * b.z);
		public static IVector3 operator /(IVector3 a, IVector3 b) => new IVector3(a.x / b.x, a.y / b.y, a.z / b.z);

		public static IVector3 operator +(IVector3 a, IVector2 b) => new IVector3(a.x + b.x, a.y + b.y, a.z);
		public static IVector3 operator -(IVector3 a, IVector2 b) => new IVector3(a.x - b.x, a.y - b.y, a.z);
		public static IVector3 operator +(IVector2 b, IVector3 a) => new IVector3(a.x + b.x, a.y + b.y, a.z);
		public static IVector3 operator -(IVector2 b, IVector3 a) => new IVector3(a.x - b.x, a.y - b.y, a.z);

		public static IVector3 operator *(IVector3 a, int b) => new IVector3(a.x * b, a.y * b, a.z * b);
		public static IVector3 operator /(IVector3 a, int b) => new IVector3(a.x / b, a.y / b, a.z / b);

		public static Vector3 operator *(IVector3 a, float b) => new Vector3(a.x * b, a.y * b, a.z * b);
		public static Vector3 operator /(IVector3 a, float b) => new Vector3(a.x / b, a.y / b, a.z / b);
	}
}
