using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class VFS
	{
		public static bool Exists(string path) => _Exists(path);
		public static string CurrentDirectory => _GetCurrentDirectory();
		public static string GetAbsolutePath(string path, bool requireFileExists = false, bool suppressWarnings = false) =>
			_GetAbsolutePath(path, requireFileExists, suppressWarnings);

		#region Mounting
		public static bool HasMount(string mountPoint) => _HasMount(mountPoint);

		public static void Mount(string mountPoint) => _Mount(mountPoint);
		public static void Mount(string mountPoint, string path) => _MountPath(mountPoint, path);

		public static void Unmount(string mountPoint) => _Unmount(mountPoint);
		public static void Unmount(string mountPoint, string path) => _UnmountPath(mountPoint, path);
		#endregion

		#region Read and Write
		public static byte[] Read(string path) => _Read(path);
		public static string ReadText(string path) => _ReadText(path);

		public static void Write(string path, byte[] data) => _Write(path, data);
		public static void Write(string path, string contents) => _WriteText(path, contents);
		#endregion

		#region File Operations
		public static void Remove(string path) => _Remove(path);
		public static void Copy(string source, string target) => _Copy(source, target);
		public static void Move(string source, string target) => _Move(source, target);
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Exists(string vfsPath);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetCurrentDirectory();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetAbsolutePath(string vfsPath, bool requireFileExists, bool suppressWarnings);

		// Mount & Unmount
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _HasMount(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Mount(string mountPoint);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unmount(string mountPoint);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _MountPath(string mountPoint, string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _UnmountPath(string mountPoint, string mountPath);

		// Read & Write
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern byte[] _Read(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Write(string path, byte[] content);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _ReadText(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _WriteText(string path, string content);

		// File Operations
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Remove(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Move(string source, string target);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Copy(string source, string target);
		#endregion
	}
}