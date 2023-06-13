using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public struct ShaderImportSettings : IImportSettings
	{
		public VFSFile VertexPath;
		public VFSFile FragmentPath;
		public VFSFile ComputePath;
		public VFSFile GeometryPath;
	}

	public class Shader : NativeResourceBase, ISerializable
	{
		public ShaderImportSettings ShaderStages
		{
			get => (ShaderImportSettings)ImportSettings;
			set => ImportSettings = value;
		}

		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);
			ResourceID = resourceID;
			Handle = handle;

			ShaderStages = new ShaderImportSettings();
			_GetStages(
				Handle,
				out string vertexPath,
				out string fragmentPath,
				out string computePath,
				out string geometryPath
			);

			ShaderStages = new ShaderImportSettings()
			{
				VertexPath	 = new VFSFile(vertexPath),
				FragmentPath = new VFSFile(fragmentPath),
				ComputePath  = new VFSFile(computePath),
				GeometryPath = new VFSFile(geometryPath)
			};
		}

		protected override void OnNativeLoad() => OnLoad();

		protected override void OnImported()
		{
			if(!TryGetImportSettings(out ShaderImportSettings importSettings))
			{
				_UnloadStages(Handle);
				return;
			}

			ShaderStages = importSettings;
			_UpdateStages(
				Handle,
				ShaderStages.VertexPath.FullPath,
				ShaderStages.FragmentPath.FullPath,
				ShaderStages.ComputePath.FullPath,
				ShaderStages.GeometryPath.FullPath
			);
		}

		public JObject OnSerialize()
		{
			return new JObject(
				new JProperty("Vertex",	  ShaderStages.VertexPath.FullPath),
				new JProperty("Fragment", ShaderStages.FragmentPath.FullPath),
				new JProperty("Compute",  ShaderStages.ComputePath.FullPath),
				new JProperty("Geometry", ShaderStages.GeometryPath.FullPath)
			);
		}

		public void OnDeserialize(JObject json) => Import(new ShaderImportSettings()
		{
			VertexPath   = json["Vertex"].Value<string>(),
			FragmentPath = json["Fragment"].Value<string>(),
			ComputePath  = json["Compute"].Value<string>(),
			GeometryPath = json["Geometry"].Value<string>()
		});

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
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Bind(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unbind(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _UnloadStages(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _UpdateStages(IntPtr handle,
																							 string vertexPath,
																							 string fragmentPath,
																							 string computePath,
																							 string geometryPath);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetStages(IntPtr handle,
																							 out string vertexPath,
																							 out string fragmentPath,
																							 out string computePath,
																							 out string geometryPath);

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