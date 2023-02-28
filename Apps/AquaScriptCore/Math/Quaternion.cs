using System.Runtime.CompilerServices;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("({x}, {y}, {z}, {w})")]
	public struct Quaternion
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
				return Quaternion.Identity;

			_FromEuler(ref values, out Quaternion output);
			return output;
		}

		public static Quaternion FromEuler(float x, float y, float z) => FromEuler(new Vector3(x, y, z));

		public void Inverse() => _InverseInline(ref this);

		public static Quaternion Inverse(Quaternion input)
		{
			_Inverse(ref input, out Quaternion output);
			return output;
		}

		public override string ToString() => $"({x}, {y}, {z}, {w})";

		public static Quaternion operator *(Quaternion a, Quaternion b)
		{
			_Multiply(ref a, ref b, out Quaternion output);
			return output;
		}

		public static Quaternion operator *(Quaternion a, Vector3 b)
		{
			_MultiplyVector3(ref a, ref b, out Quaternion output);
			return output;
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Multiply(ref Quaternion a, ref Quaternion b, out Quaternion output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _MultiplyVector3(ref Quaternion a, ref Vector3 b, out Quaternion output);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _FromEuler(ref Vector3 input, out Quaternion output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _ToEuler(ref Quaternion input, out Vector3 output);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Inverse(ref Quaternion input, out Quaternion output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _InverseInline(ref Quaternion input);
		#endregion

	}
}
