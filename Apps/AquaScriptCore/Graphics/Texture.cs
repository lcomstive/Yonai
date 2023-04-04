using System;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public class Texture : NativeResourceBase
	{
		public IVector2 Resolution
		{
			get
			{
				_GetResolution(Handle, out IVector2 result);
				return result;
			}
		}

		internal override bool Load(string path, params object[] args)
		{
			ulong resourceID;
			IntPtr handle;

			if (args.Length >= 1)
				_Load1(path, (string)args[0], args.Length >= 2 ? (bool)args[2] : false, out resourceID, out handle);
			else
				_Load0(path, out resourceID, out handle);

			ResourceID = resourceID;
			Handle = handle;
			return true;
		}

		public void Bind(uint index = 0) => _Bind(Handle, index);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load0(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load1(string path, string filePath, bool hdr, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Bind(IntPtr handle, uint index);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetResolution(IntPtr handle, out IVector2 resolution);
		#endregion
	}
}