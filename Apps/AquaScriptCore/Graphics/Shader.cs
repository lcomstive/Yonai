using System;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public struct ShaderStages
	{
		public string VertexPath;
		public string FragmentPath;
		public string ComputePath;
		public string GeomtryPath;
	}

	public class Shader : NativeResourceBase
	{
		internal override bool Load(string path, params object[] args)
		{
			uint resourceID;
			IntPtr handle;

			if (args.Length >= 1)
			{
				if(!(args[0] is ShaderStages))
				{
					Log.Error("Shader requires argument of type AquaEngine.Graphics.ShaderStages");
					return false;
				}
				ShaderStages stages = (ShaderStages)args[0];
				_Load1(path, stages.VertexPath, stages.FragmentPath, stages.ComputePath, stages.GeomtryPath, out resourceID, out handle);
			}
			else
				_Load0(path, out resourceID, out handle);

			ResourceID = resourceID;
			Handle = handle;
			return true;
		}

		public void Bind() => _Bind(Handle);
		public void Unbind() => _Unbind(Handle);

		public void Set(int location, int value) => _Set_int(Handle, location, value);
		public void Set(int location, bool value) => _Set_bool(Handle, location, value);
		public void Set(int location, float value) => _Set_float(Handle, location, value);
		public void Set(int location, double value) => _Set_double(Handle, location, value);
		public void Set(int location, Vector2 value) => _Set_vec2(Handle, location, value);
		public void Set(int location, Vector3 value) => _Set_vec3(Handle, location, value);
		public void Set(int location, Vector4 value) => _Set_vec4(Handle, location, value);
		public void Set(int location, IVector2 value) => _Set_vec2(Handle, location, (Vector2)value);
		public void Set(int location, IVector3 value) => _Set_vec3(Handle, location, (Vector3)value);
		public void Set(int location, IVector4 value) => _Set_vec4(Handle, location, (Vector4)value);
	
		public void Set(string location, int value) => _SetStr_int(Handle, location, value);
		public void Set(string location, bool value) => _SetStr_bool(Handle, location, value);
		public void Set(string location, float value) => _SetStr_float(Handle, location, value);
		public void Set(string location, double value) => _SetStr_double(Handle, location, value);
		public void Set(string location, Vector2 value) => _SetStr_vec2(Handle, location, value);
		public void Set(string location, Vector3 value) => _SetStr_vec3(Handle, location, value);
		public void Set(string location, Vector4 value) => _SetStr_vec4(Handle, location, value);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load0(string path, out uint resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load1(string path,
																							 string vertexPath,
																							 string fragmentPath,
																							 string computePath,
																							 string geometryPath,
																							 out uint resourceID,
																							 out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Bind(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unbind(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void UpdateStages(IntPtr handle,
																							 string vertexPath,
																							 string fragmentPath,
																							 string computePath,
																							 string geometryPath);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_int(IntPtr handle, int location, int value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_int(IntPtr handle, string location, int value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_bool(IntPtr handle, int location, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_bool(IntPtr handle, string location, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_float(IntPtr handle, int location, float value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_float(IntPtr handle, string location, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_double(IntPtr handle, int location, double value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_double(IntPtr handle, string location, double value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_vec2(IntPtr handle, int location, Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_vec2(IntPtr handle, string location, Vector2 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_vec3(IntPtr handle, int location, Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_vec3(IntPtr handle, string location, Vector3 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_vec4(IntPtr handle, int location, Vector4 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_vec4(IntPtr handle, string location, Vector4 value);

		/*
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_mat3(IntPtr handle, int location, mat3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_mat3(IntPtr handle, string location, mat3 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Set_mat4(IntPtr handle, int location, mat4 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetStr_mat4(IntPtr handle, string location, mat4 value);
		*/
		#endregion
	}
}