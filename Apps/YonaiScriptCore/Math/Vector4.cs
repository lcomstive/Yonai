using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace Yonai
{
	[System.Diagnostics.DebuggerDisplay("({x}, {y}, {z}, {w})")]
	public struct Vector4 : ISerializable
	{
		public float x, y, z, w;

		public float Magnitude => _Magnitude(ref this);

		public static Vector4 Zero		=> new Vector4(0, 0, 0, 0);
		public static Vector4 One		=> new Vector4(1, 1, 1, 1);

		public Vector4(float x = 0, float y = 0, float z = 0, float w = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
		}

		public Vector4(float scalar) => x = y = z = w = scalar;

		public static Vector4 Lerp(Vector4 a, Vector4 b, float time) =>
			new Vector4(
				MathUtils.Lerp(a.x, b.x, time),
				MathUtils.Lerp(a.y, b.y, time),
				MathUtils.Lerp(a.z, b.z, time),
				MathUtils.Lerp(a.w, b.w, time)
				);

		public override string ToString() => $"({x}, {y}, {z}, {w})";

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("x", x),
				new JProperty("y", y),
				new JProperty("z", z),
				new JProperty("w", w)
				);

		public void OnDeserialize(JObject json)
		{
			x = json["x"].Value<float>();
			y = json["y"].Value<float>();
			z = json["z"].Value<float>();
			w = json["w"].Value<float>();
		}

		public static implicit operator string(Vector4 v) => v.ToString();
		public static implicit operator Vector4(IVector4 v) => new Vector4(v.x, v.y, v.z, v.w);

		public static Vector4 operator +(Vector4 a, Vector4 b) => new Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		public static Vector4 operator -(Vector4 a, Vector4 b) => new Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		public static Vector4 operator *(Vector4 a, Vector4 b) => new Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		public static Vector4 operator /(Vector4 a, Vector4 b) => new Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);

		public static Vector4 operator *(Vector4 a, float b) => new Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
		public static Vector4 operator /(Vector4 a, float b) => new Vector4(a.x / b, a.y / b, a.z / b, a.w / b);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _Magnitude(ref Vector4 input);
		#endregion

		public float this[int i]
		{
			get
			{
				switch(i)
				{
					default:
					case 0: return x;
					case 1: return y;
					case 2: return z;
					case 3: return w;
				}
			}
		}
	}
}
