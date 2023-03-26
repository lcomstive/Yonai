namespace AquaEngine
{
	public static class MathUtils
	{
		public static float Lerp(float a, float b, float t)
		{
			t = Clamp(t, 0.0f, 1.0f);
			return (1.0f - t) * a + t * b;
		}

		public static float Clamp(float a, float min, float max) => (a < min) ? min : ((a > max) ? max : a);
	}
	
	public static class Random
	{
		public static float Range(float min, float max)
		{
			System.Random rand = new System.Random();
			return (float)rand.NextDouble() * (max - min) + min;
		}

		public static float Range(float max) => Range(0, max);
	}
}
