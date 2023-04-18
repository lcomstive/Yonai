using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("({x}, {y})")]
	public struct IVector2 : ISerializable
	{
		public int x, y;

		public static IVector2 Zero => new IVector2(0, 0);
		public static IVector2 One => new IVector2(1, 1);

		public IVector2(int x = 0, int y = 0)
		{
			this.x = x;
			this.y = y;
		}

		public static IVector2 Lerp(IVector2 a, IVector2 b, float time) =>
			new IVector2(
				(int)Math.Round(MathUtils.Lerp(a.x, b.x, time), 0),
				(int)Math.Round(MathUtils.Lerp(a.y, b.y, time), 0)
				);

		public override string ToString() => $"({x}, {y})";

		public static explicit operator IVector2(Vector2 v) => new IVector2((int)Math.Floor(v.x), (int)Math.Floor(v.y));
		public static implicit operator string(IVector2 v) => v.ToString();

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("x", x),
				new JProperty("y", y)
				);

		public void OnDeserialize(JObject json)
		{
			x = json["x"].Value<int>();
			y = json["y"].Value<int>();
		}

		public static IVector2 operator +(IVector2 a, IVector2 b) => new IVector2(a.x + b.x, a.y + b.y);
		public static IVector2 operator -(IVector2 a, IVector2 b) => new IVector2(a.x - b.x, a.y - b.y);
		public static IVector2 operator *(IVector2 a, IVector2 b) => new IVector2(a.x * b.x, a.y * b.y);
		public static IVector2 operator /(IVector2 a, IVector2 b) => new IVector2(a.x / b.x, a.y / b.y);

		public static IVector2 operator *(IVector2 a, int b) => new IVector2(a.x * b, a.y * b);
		public static IVector2 operator /(IVector2 a, int b) => new IVector2(a.x / b, a.y / b);

		public static Vector2 operator *(IVector2 a, float b) => new Vector2(a.x * b, a.y * b);
		public static Vector2 operator /(IVector2 a, float b) => new Vector2(a.x / b, a.y / b);
	}
}
