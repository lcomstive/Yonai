﻿using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public struct Vector2
	{
		public float x, y;

		public float Magnitude => _Magnitude(ref this);

		public static Vector2 Zero => new Vector2(0, 0);
		public static Vector2 One  => new Vector2(1, 1);

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

		public static implicit operator string(Vector2 v) => v.ToString();

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
