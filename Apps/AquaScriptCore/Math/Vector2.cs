using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public struct Vector2
	{
		public float x, y;

		public float Magnitude => _aqua_internal_Magnitude(ref this);

		public static Vector3 Zero => new Vector3(0, 0, 0);
		public static Vector3 One => new Vector3(1, 1, 1);

		public Vector2(float x = 0, float y = 0)
		{
			this.x = x;
			this.y = y;
		}

		public float Distance(Vector2 other) => _aqua_internal_Distance(ref this, ref other);
		public static float Distance(Vector2 a, Vector2 b) => _aqua_internal_Distance(ref a, ref b);

		public override string ToString() => "(" + x + ", " + y + ")";

		public static implicit operator string(Vector2 v) => v.ToString();

		public static Vector2 operator +(Vector2 a, Vector2 b) => new Vector2(a.x + b.x, a.y + b.y);
		public static Vector2 operator -(Vector2 a, Vector2 b) => new Vector2(a.x - b.x, a.y - b.y);
		public static Vector2 operator *(Vector2 a, Vector2 b) => new Vector2(a.x * b.x, a.y * b.y);
		public static Vector2 operator /(Vector2 a, Vector2 b) => new Vector2(a.x / b.x, a.y / b.y);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_Magnitude(ref Vector2 input);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_Distance(ref Vector2 a, ref Vector2 b);
		#endregion
	}
}
