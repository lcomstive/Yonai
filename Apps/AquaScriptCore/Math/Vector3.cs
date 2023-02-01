using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public struct Vector3
	{
		public float x, y, z;

		public float Magnitude => _Magnitude(ref this);

		public static Vector3 Zero		=> new Vector3(0, 0, 0);
		public static Vector3 One		=> new Vector3(1, 1, 1);

		public static Vector3 Left		=> new Vector3( 1,  0,  0);
		public static Vector3 Right		=> new Vector3(-1,  0,  0);
		public static Vector3 Up		=> new Vector3( 0,  1,  0);
		public static Vector3 Down		=> new Vector3( 0, -1,  0);
		public static Vector3 Forward	=> new Vector3( 0,  0,  1);
		public static Vector3 Back		=> new Vector3( 0,  0, -1);

		public Vector3(float x = 0, float y = 0 , float z = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public Vector3(float scalar) => x = y = z = scalar;

		public float Dot(Vector3 other) => _Dot(ref this, ref other);
		public static float Dot(Vector3 a, Vector3 b) => _Dot(ref a, ref b);

		public float Distance(Vector3 other) => _Distance(ref this, ref other);
		public static float Distance(Vector3 a, Vector3 b) => _Distance(ref a, ref b);

		public static Vector3 Lerp(Vector3 a, Vector3 b, float time) =>
			new Vector3(
				MathUtils.Lerp(a.x, b.x, time),
				MathUtils.Lerp(a.y, b.y, time),
				MathUtils.Lerp(a.z, b.z, time)
				);

		public override string ToString() => $"({x}, {y}, {z})";

		public static implicit operator string(Vector3 v) => v.ToString();

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
