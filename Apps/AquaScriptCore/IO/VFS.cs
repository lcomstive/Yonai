using System.IO;
using System.Runtime.CompilerServices;

namespace AquaEngine.IO
{
	public struct VFSFile
	{
		/// <summary>
		/// Name of file (including extension) or directory
		/// </summary>
		public string FileName { get; private set; }

		/// <summary>
		/// Extension of the file, or <see cref="string.Empty"/> if directory
		/// </summary>
		public string Extension { get; private set; }

		/// <summary>
		/// True when file is directory
		/// </summary>
		public bool IsDirectory { get; private set; }

		/// <summary>
		/// Path to directory containing this file
		/// </summary>
		public string ParentDirectory { get; private set; }

		public VFSFile(string filepath)
		{
			IsDirectory = filepath.EndsWith("/");
			filepath = filepath.TrimEnd('/');
			if (!IsDirectory)
			{
				FileName = Path.GetFileName(filepath);
				Extension = Path.GetExtension(filepath);
			}
			else
			{
				Extension = string.Empty;
				FileName = filepath.Substring(filepath.LastIndexOf("/") + 1);
			}
			ParentDirectory = filepath.Substring(0, filepath.LastIndexOf('/') + 1);
		}

		public string FullPath => ParentDirectory + FileName;
	}

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

		public static bool CreateDirectory(string path) => _CreateDirectory(path);
		public static bool DeleteDirectory(string path) => _DeleteDirectory(path);

		public static VFSFile[] GetFiles(string directory, bool recursive = false)
		{
			string[] filepaths = _GetFiles(directory, recursive);
			VFSFile[] files = new VFSFile[filepaths.Length];
			for(int i = 0; i < filepaths.Length; i++)
				files[i] = new VFSFile(filepaths[i]);
			return files;
		}
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
		
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string[] _GetFiles(string directory, bool recursive);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _CreateDirectory(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _DeleteDirectory(string path);
		#endregion
	}
}