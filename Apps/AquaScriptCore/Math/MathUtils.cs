using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class MathUtils
	{
		public const float Pi = 3.1415926535897931f;

		public static float Lerp(float a, float b, float t)
		{
			t = Clamp(t, 0.0f, 1.0f);
			return (1.0f - t) * a + t * b;
		}

		public static int Clamp(int a, int min, int max) => (a < min) ? min : ((a > max) ? max : a);
		public static uint Clamp(uint a, uint min, uint max) => (a < min) ? min : ((a > max) ? max : a);
		public static float Clamp(float a, float min, float max) => (a < min) ? min : ((a > max) ? max : a);

		public static float Rad2Deg(float rads) => rads * (180.0f / Pi);
		public static float Deg2Rad(float deg) => deg * (Pi / 180.0f);

		public static Vector3 Rad2Deg(Vector3 rad) => rad * (180.0f / Pi);
		public static Vector3 Deg2Rad(Vector3 deg) => deg * (Pi / 180.0f);
	}
	
	public static class Random
	{
		public static float Range(float max) => _Float(0, max);
		public static float Range(float min, float max) => _Float(min, max);

		public static int Range(int max) => _Int(0, max);
		public static int Range(int min, int max) => _Int(min, max);

		public static T Range<T>(T[] values) => values[Range(values.Length)];
		public static T Range<T>(List<T> values) => values[Range(values.Count)];

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _Int(int min, int max);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _Float(float min, float max);
	}
}
