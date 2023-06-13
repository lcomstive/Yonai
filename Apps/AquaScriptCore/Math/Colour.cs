using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System.Diagnostics;
using System.Globalization;

namespace AquaEngine
{
	[DebuggerDisplay("({r}, {g}, {b}, {a})")]
	public struct Colour : ISerializable
	{
		public float r, g, b, a;

		public static readonly Colour Black = new Colour(0, 0, 0);
		public static readonly Colour White = new Colour(1, 1, 1);
		public static readonly Colour Grey = new Colour(.5f, .5f, .5f);
		public static readonly Colour Red = new Colour(1, 0, 0);
		public static readonly Colour Green = new Colour(0, 1, 0);
		public static readonly Colour Blue = new Colour(0, 0, 1);
		public static readonly Colour Yellow = new Colour(1, 1, 0);
		public static readonly Colour Magenta = new Colour(1, 0, 1);
		public static readonly Colour Purple = new Colour(.5f, 0, 1);
		public static readonly Colour Orange = new Colour(1, .5f, 1);
		public static readonly Colour Turqoise = new Colour(0, 1, 1);

		public static readonly Colour None = new Colour(0, 0, 0, 0);

		public Colour(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
		{
			this.r = r;
			this.g = g;
			this.b = b;
			this.a = a;
		}

		public static Colour Lerp(Colour a, Colour b, float time) =>
			new Colour(
				MathUtils.Lerp(a.r, b.r, time),
				MathUtils.Lerp(a.g, b.g, time),
				MathUtils.Lerp(a.b, b.b, time),
				MathUtils.Lerp(a.a, b.a, time)
				);

		/// <returns>Converted colour, or <see cref="Black"/> if invalid</returns>
		public static Colour FromHex(string hex)
		{
			if (string.IsNullOrWhiteSpace(hex))
				return Black;

			// Remove '#' from start, if present
			hex = hex.TrimStart('#');

			// Check for alpha in hex
			if (hex.Length == 6)
				hex += "FF"; // Append full alpha value (255)

			if (!int.TryParse(hex, NumberStyles.HexNumber, CultureInfo.CurrentCulture, out int hexRaw))
			{
				Log.Warning($"Failed to parse hex '{hex}'");
				return Black;
			}

			return new Colour(
				(hexRaw & 0xff000000) >> 24,
				(hexRaw & 0xff0000) >> 16,
				(hexRaw & 0xff00) >> 8,
				hexRaw & 0xff
			) / 255.0f; // (convert from range [0 - 255] -> [0.0 - 1.0]
		}

		public JObject OnSerialize() =>
			new JObject(
				new JProperty("r", r),
				new JProperty("g", g),
				new JProperty("b", b),
				new JProperty("a", a)
				);

		public void OnDeserialize(JObject json)
		{
			r = json["r"].Value<float>();
			g = json["g"].Value<float>();
			b = json["b"].Value<float>();
			a = json["a"].Value<float>();
		}

		public override bool Equals(object obj)
		{
			if (!(obj is Colour) && !(obj is Vector4))
				return false;
			return this == (Colour)obj;
		}

		public override string ToString() => $"({r}, {g}, {b}, {a})";

		public static implicit operator string(Colour v) => v.ToString();

		public static Colour operator +(Colour a, Colour b) => new Colour(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
		public static Colour operator -(Colour a, Colour b) => new Colour(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
		public static Colour operator *(Colour a, Colour b) => new Colour(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
		public static Colour operator /(Colour a, Colour b) => new Colour(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a);

		public static Colour operator *(Colour a, float b) => new Colour(a.r * b, a.g * b, a.b * b, a.a * b);
		public static Colour operator /(Colour a, float b) => new Colour(a.r / b, a.g / b, a.b / b, a.a / b);

		public static bool operator ==(Colour a, Colour b) => a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
		public static bool operator !=(Colour a, Colour b) => a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
		
		public static implicit operator Vector4(Colour a) => new Vector4(a.r, a.g, a.b, a.a);
		public static implicit operator Colour(Vector4 a) => new Colour(a.x, a.y, a.z, a.w);
	}
}
