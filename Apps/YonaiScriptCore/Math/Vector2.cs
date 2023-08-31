using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace Yonai
{
	public struct Vector2 : ISerializable
	{
		public float x, y;

		public float Magnitude => _Magnitude(ref this);

		public static Vector2 Zero => new Vector2(0, 0);
		public static Vector2 One  => new Vector2(1, 1);

		public static Vector2 Up  => new Vector2(0, 1);
		public static Vector2 Right  => new Vector2(1, 0);

		public Vector2(float x = 0, float y = 0)
		{
			this.x = x;
			this.y = y;
		}

		public float Distance(Vector2 other) => _Distance(ref this, ref other);
		public static float Distance(Vector2 a, Vector2 b) => _Distance(ref a, ref b);

		public static Vector2 Lerp(Vector2 a, Vector2 b, float time) =>
			new Vector2(
				MathUtils.Lerp(a.x, b.x, time),
				MathUtils.Lerp(a.y, b.y, time)
				);

		public override string ToString() => "(" + x + ", " + y + ")";

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("x", x),
				new JProperty("y", y)
				);

		public void OnDeserialize(JObject json)
		{
			x = json["x"]?.Value<float>() ?? 0;
			y = json["y"]?.Value<float>() ?? 0;
		}

		public static implicit operator string(Vector2 v) => v.ToString();
		public static implicit operator Vector2(IVector2 v) => new Vector2(v.x, v.y);

		public static Vector2 operator +(Vector2 a, Vector2 b) => new Vector2(a.x + b.x, a.y + b.y);
		public static Vector2 operator -(Vector2 a, Vector2 b) => new Vector2(a.x - b.x, a.y - b.y);
		public static Vector2 operator *(Vector2 a, Vector2 b) => new Vector2(a.x * b.x, a.y * b.y);
		public static Vector2 operator /(Vector2 a, Vector2 b) => new Vector2(a.x / b.x, a.y / b.y);

		public static Vector2 operator *(Vector2 a, float b) => new Vector2(a.x * b, a.y * b);
		public static Vector2 operator /(Vector2 a, float b) => new Vector2(a.x / b, a.y / b);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _Magnitude(ref Vector2 input);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _Distance(ref Vector2 a, ref Vector2 b);
		#endregion
	}
}
