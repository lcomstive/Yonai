using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace Yonai
{
	[System.Diagnostics.DebuggerDisplay("({x}, {y}, {z})")]
	public struct Vector3 : ISerializable
	{
		public float x, y, z;

		/// <summary>
		/// The combined length of the vector
		/// </summary>
		public float Magnitude => _Magnitude(ref this);

		public static Vector3 Zero		=> new Vector3(0, 0, 0);
		public static Vector3 One		=> new Vector3(1, 1, 1);

		/// <summary>
		/// Left in global space
		/// </summary>
		public static Vector3 Left		=> new Vector3( 1,  0,  0);

		/// <summary>
		/// Right in global space
		/// </summary>
		public static Vector3 Right		=> new Vector3(-1,  0,  0);

		/// <summary>
		/// Up in global space
		/// </summary>
		public static Vector3 Up		=> new Vector3( 0,  1,  0);

		/// <summary>
		/// Down in global space
		/// </summary>
		public static Vector3 Down		=> new Vector3( 0, -1,  0);

		/// <summary>
		/// Forwards in global space
		/// </summary>
		public static Vector3 Forward	=> new Vector3( 0,  0,  1);

		/// <summary>
		/// Backwards in global space
		/// </summary>
		public static Vector3 Back		=> new Vector3( 0,  0, -1);

		public Vector3(float x = 0, float y = 0 , float z = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public Vector3(Vector3 other)
		{
			Log.Warning("COPY");
			x = other.x;
			y = other.y;
			z = other.z;
		}

		/// <summary>
		/// Creates a vector with x, y and z components all set to <paramref name="scalar"/>
		/// </summary>
		public Vector3(float scalar) => x = y = z = scalar;

		public float Dot(Vector3 other) => _Dot(ref this, ref other);
		public static float Dot(Vector3 a, Vector3 b) => _Dot(ref a, ref b);

		/// <returns>Distance between this vector and another</returns>
		public float Distance(Vector3 other) => _Distance(ref this, ref other);

		/// <returns>Distance between two vectors</returns>
		public static float Distance(Vector3 a, Vector3 b) => _Distance(ref a, ref b);

		/// <summary>
		/// Linearly interpolates from <paramref name="a"/> to <paramref name="b"/>.
		/// <paramref name="time"/> is percentage of interpolation between the two values.
		/// </summary>
		public static Vector3 Lerp(Vector3 a, Vector3 b, float time) =>
			new Vector3(
				MathUtils.Lerp(a.x, b.x, time),
				MathUtils.Lerp(a.y, b.y, time),
				MathUtils.Lerp(a.z, b.z, time)
				);

		public override string ToString() => $"({x}, {y}, {z})";

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("x", x),
				new JProperty("y", y),
				new JProperty("z", z)
				);

		public void OnDeserialize(JObject json)
		{
			x = json["x"].Value<float>();
			y = json["y"].Value<float>();
			z = json["z"].Value<float>();
		}

		public static implicit operator string(Vector3 v) => v.ToString();
		public static implicit operator Vector3(IVector3 v) => new Vector3(v.x, v.y, v.z);

		public static Vector3 operator -(Vector3 a) => new Vector3(-a.x, -a.y, -a.z);

		public static Vector3 operator +(Vector3 a, Vector3 b) => new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
		public static Vector3 operator -(Vector3 a, Vector3 b) => new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
		public static Vector3 operator *(Vector3 a, Vector3 b) => new Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
		public static Vector3 operator /(Vector3 a, Vector3 b) => new Vector3(a.x / b.x, a.y / b.y, a.z / b.z);

		public static Vector3 operator *(Vector3 a, float b) => new Vector3(a.x * b, a.y * b, a.z * b);
		public static Vector3 operator /(Vector3 a, float b) => new Vector3(a.x / b, a.y / b, a.z / b);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _Magnitude(ref Vector3 input);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _Dot(ref Vector3 a, ref Vector3 b);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _Distance(ref Vector3 a, ref Vector3 b);
		#endregion
	}
}
