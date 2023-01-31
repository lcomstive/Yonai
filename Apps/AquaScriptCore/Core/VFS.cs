using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class VFS
	{
		public static bool Exists(string path) => _aqua_internal_Exists(path);
		public static string CurrentDirectory => _aqua_internal_GetCurrentDirectory();
		public static string GetAbsolutePath(string path) => _aqua_internal_GetAbsolutePath(path);

		#region Mounting
		public static bool HasMount(string mountPoint) => _aqua_internal_HasMount(mountPoint);

		public static void Mount(string mountPoint) => _aqua_internal_Mount(mountPoint);
		public static void Mount(string mountPoint, string path) => _aqua_internal_MountPath(mountPoint, path);

		public static void Unmount(string mountPoint) => _aqua_internal_Unmount(mountPoint);
		public static void Unmount(string mountPoint, string path) => _aqua_internal_UnmountPath(mountPoint, path);
		#endregion

		#region Read and Write
		public static byte[] Read(string path) => _aqua_internal_Read(path);
		public static string ReadText(string path) => _aqua_internal_ReadText(path);

		public static void Write(string path, byte[] data) => _aqua_internal_Write(path, data);
		public static void Write(string path, string contents) => _aqua_internal_WriteText(path, contents);
		#endregion

		#region File Operations
		public static void Remove(string path) => _aqua_internal_Remove(path);
		public static void Copy(string source, string target) => _aqua_internal_Copy(source, target);
		public static void Move(string source, string target) => _aqua_internal_Move(source, target);
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_Exists(string vfsPath);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _aqua_internal_GetCurrentDirectory();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _aqua_internal_GetAbsolutePath(string vfsPath);

		// Mount & Unmount
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_HasMount(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Mount(string mountPoint);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Unmount(string mountPoint);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_MountPath(string mountPoint, string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_UnmountPath(string mountPoint, string mountPath);

		// Read & Write
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern byte[] _aqua_internal_Read(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Write(string path, byte[] content);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _aqua_internal_ReadText(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_WriteText(string path, string content);

		// File Operations
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Remove(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Move(string source, string target);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Copy(string source, string target);
		#endregion
	}
}