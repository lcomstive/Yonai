using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace Yonai
{
	public struct Matrix4 : ISerializable
	{
		public float[] Components;

		public static Matrix4 Identity = new Matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);

		public Matrix4 transposed
		{
			get
			{
				Matrix4 value = new Matrix4(this);
				value.Tranpose();
				return value;
			}
		}

		public Matrix4(Matrix4 other) : this(other.Components) { }
		public Matrix4(float[] components)
		{
			Components = new float[4 * 4];

			// Copy components into m_Components
			Array.Copy(components, Components, System.Math.Min(components.Length, Components.Length));

			// Fill remainder of array if input components are incomplete
			for(int i = components.Length;i < Components.Length;i++)
				Components[i] = 0;
		}

		public Matrix4(float m00, float m10, float m20, float m30,
					   float m01, float m11, float m21, float m31,
					   float m02, float m12, float m22, float m32,
					   float m03, float m13, float m23, float m33)
		{
			Components = new float[]
			{
				m00, m10, m20, m30,
				m01, m11, m21, m31,
				m02, m12, m22, m32,
				m03, m13, m23, m33
			};
		}

		/// <summary>
		/// Converts a column-major matrix to a row-major, and vice versa
		/// </summary>
		public void Tranpose()
		{
			Matrix4 temp = new Matrix4(this);
			for(int i = 0;i < 4;i++)
				for(int j = 0;j < 4;j++)
					this[i, j] = temp[j, i];
		}

		public JObject OnSerialize() =>
			new JObject(Components);

		public void OnDeserialize(JObject json) => Components = json.Value<float[]>();

		public Matrix4 Rotate(float angle, Vector3 axis)
		{
			_Rotate(ref this, angle, ref axis, out Matrix4 output);
			return output;
		}

		public static Matrix4 LookAt(Vector3 eye, Vector3 direction, Vector3 up)
		{
			_LookAt(ref eye, ref direction, ref up, out Matrix4 output);
			return output;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="fieldOfView">Field of view, in degrees</param>
		/// <param name="aspectRatio"></param>
		/// <param name="zNear"></param>
		/// <param name="zFar"></param>
		/// <returns></returns>
		public static Matrix4 Perspective(float fieldOfView, float aspectRatio, float zNear = 0.1f, float zFar = 100)
		{
			_Perspective(fieldOfView, aspectRatio, zNear, zFar, out Matrix4 output);
			return output;
		}

		#region Conversions & Operators
		public static Vector4 operator *(Matrix4 m, Vector4 v) => new Vector4(
			 m[0, 0] * v[0] + m[0, 1] * v[1] + m[0, 2] * v[2] + m[0, 3] * v[3],
			 m[1, 0] * v[0] + m[1, 1] * v[1] + m[1, 2] * v[2] + m[1, 3] * v[3],
			 m[2, 0] * v[0] + m[2, 1] * v[1] + m[2, 2] * v[2] + m[2, 3] * v[3],
			 m[3, 0] * v[0] + m[3, 1] * v[1] + m[3, 2] * v[2] + m[3, 3] * v[3]
			 );

		public static Vector4 operator *(Vector4 v, Matrix4 m) => new Vector4(
			m[0, 0] * v[0] + m[1, 0] * v[1] + m[2, 0] * v[2] + m[3, 0] * v[3],
			m[0, 1] * v[0] + m[1, 1] * v[1] + m[2, 1] * v[2] + m[3, 1] * v[3],
			m[0, 2] * v[0] + m[1, 2] * v[1] + m[2, 2] * v[2] + m[3, 2] * v[3],
			m[0, 3] * v[0] + m[1, 3] * v[1] + m[2, 3] * v[2] + m[3, 3] * v[3]
			);

		public static Matrix4 operator *(Matrix4 a, Matrix4 b)
		{
			float[] components = new float[4 * 4];
			for(int i = 0;i < 4;i++)
				for(int j = 0;j < 4;j++)
					for(int k = 0;k < 4;k++)
						components[i + j * 4] += a.Components[i + k * 4] * b.Components[k + j * 4];
			return new Matrix4(components);
		}

		public string ToString(bool useNewLine)
		{
			string s = useNewLine ? Environment.NewLine : string.Empty;
			for(int i = 0;i < 4;i++)
			{
				for(int j = 0;j < 4;j++)
					s += Components[j + i * 4] + (j < 3 ? ", " : "");
				if(useNewLine)
					s += Environment.NewLine;
			}
			return $"[{s}]";
		}

		public override string ToString() => ToString(true);

		public Vector4 this[int i]
		{
			get => i >= 4 || i < 0 ? Vector4.Zero : new Vector4(Components[i], Components[i + 4], Components[i + 8], Components[i + 12]);
			set
			{
				if(i >= 4 || i < 0)
					return;
				Components[i] = value.x;
				Components[i + 4] = value.y;
				Components[i + 8] = value.z;
				Components[i + 12] = value.w;
			}
		}

		public float this[int i, int j]
		{
			get => (i >= 4 || j >= 4 || i < 0 || j < 0) ? 0 : Components[i + (j * 4)];
			set
			{
				if(i >= 4 || j >= 4 || i < 0 || j < 0)
					return;
				Components[i + (j * 4)] = value;
			}
		}
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _LookAt(ref Vector3 eye, ref Vector3 direction, ref Vector3 up, out Matrix4 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Perspective(float fov, float aspect, float near, float far, out Matrix4 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Translate(ref Matrix4 input, ref Vector3 translation, out Matrix4 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Scale(ref Matrix4 input, ref Vector3 scale, out Matrix4 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _ToMatrix(ref Quaternion input, out Matrix4 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Invert(ref Matrix4 input, out Matrix4 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Rotate(ref Matrix4 input, float angle, ref Vector3 axis, out Matrix4 output);
		#endregion
	}
}

