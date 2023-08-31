using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace Yonai
{
	[System.Diagnostics.DebuggerDisplay("({x}, {y}, {z}, {w})")]
	public struct Quaternion : ISerializable
	{
		public float x, y, z, w;

		public static Quaternion Identity => new Quaternion(0, 0, 0, 1);

		public Vector3 Euler
		{
			get
			{
				Quaternion input = this;
				_ToEuler(ref input, out Vector3 output);
				return output;
			}
			set => _FromEuler(ref value, out this);
		}

		public Quaternion Normalised
		{
			get
			{
				Quaternion copy = this;
				copy.Normalise();
				return copy;
			}
		}

		public Quaternion Inverted
		{
			get
			{
				_Inverse(ref this, out Quaternion output);
				return output;
			}
		}

		public Quaternion(float x, float y, float z, float w)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
		}

		public static Quaternion FromEuler(Vector3 values)
		{
			if(values == Vector3.Zero)
				return Identity;

			_FromEuler(ref values, out Quaternion output);
			return output;
		}

		public static Quaternion FromEuler(float x, float y, float z) => FromEuler(new Vector3(x, y, z));

		public void Inverse() => _InverseInline(ref this);

		public void Normalise() => _Normalise(ref this);

		public static Quaternion AngleAxis(float angle, Vector3 axis)
		{
			_AngleAxis(angle, ref axis, out Quaternion output);
			return output;
		}

		public override string ToString() => $"({x}, {y}, {z}, {w})";

		public static Quaternion operator *(Quaternion a, Quaternion b)
		{
			_Multiply(ref a, ref b, out Quaternion output);
			return output;
		}

		public static Vector3 operator *(Quaternion a, Vector3 b)
		{
			_MultiplyVector3(ref a, ref b, out Vector3 output);
			return output;
		}

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

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Multiply(ref Quaternion a, ref Quaternion b, out Quaternion output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _MultiplyVector3(ref Quaternion a, ref Vector3 b, out Vector3 output);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _FromEuler(ref Vector3 input, out Quaternion output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _ToEuler(ref Quaternion input, out Vector3 output);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Inverse(ref Quaternion input, out Quaternion output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _InverseInline(ref Quaternion input);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Normalise(ref Quaternion instance);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AngleAxis(float angle, ref Vector3 axis, out Quaternion output);
		#endregion

	}
}
