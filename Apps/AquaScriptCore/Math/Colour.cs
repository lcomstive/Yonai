using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public struct Colour
	{
		public float r, g, b, a;

		public static Colour Black = new Colour(0, 0, 0);
		public static Colour White = new Colour(1, 1, 1);
		public static Colour Red = new Colour(1, 0, 0);
		public static Colour Green = new Colour(0, 1, 0);
		public static Colour Blue = new Colour(0, 0, 1);

		public Colour(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
		{
			this.r = r;
			this.g = g;
			this.b = b;
			this.a = a;
		}

		public Colour(float scalar) => r = g = b = a = scalar;

		public static Colour Lerp(Colour a, Colour b, float time) =>
			new Colour(
				MathUtils.Lerp(a.r, b.r, time),
				MathUtils.Lerp(a.g, b.g, time),
				MathUtils.Lerp(a.b, b.b, time),
				MathUtils.Lerp(a.a, b.a, time)
				);

		public override string ToString() => $"({r}, {g}, {b}, {a})";

		public static implicit operator string(Colour v) => v.ToString();

		public static Colour operator +(Colour a, Colour b) => new Colour(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
		public static Colour operator -(Colour a, Colour b) => new Colour(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
		public static Colour operator *(Colour a, Colour b) => new Colour(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
		public static Colour operator /(Colour a, Colour b) => new Colour(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a);

		public static Colour operator *(Colour a, float b) => new Colour(a.r * b, a.g * b, a.b * b, a.a * b);
		public static Colour operator /(Colour a, float b) => new Colour(a.r / b, a.g / b, a.b / b, a.a / b);
	}
}
