using AquaEngine.IO;
using System.Runtime.CompilerServices;

namespace AquaEditor
{
	public static class FileDialog
	{
		public static readonly string[] DefaultFilters = new string[]
		{
			"All Files",
			"*"
		};

		public static string OpenFolder(string title, string initialDir = "")
			=> _OpenFolder(title, VFS.GetAbsolutePath(initialDir));

		public static string[] OpenFile(string title, string initialDir = "", bool multiselect = false)
			=> _OpenFile(title, VFS.GetAbsolutePath(initialDir), DefaultFilters, multiselect);

		public static string[] OpenFile(string title, string initialDir = "", bool multiselect = false, params string[] fileFilters)
			=> _OpenFile(title, VFS.GetAbsolutePath(initialDir), fileFilters, multiselect);

		public static string[] OpenFile(string title, string initialDir = "", params string[] fileFilters)
			=> OpenFile(title, VFS.GetAbsolutePath(initialDir), false, fileFilters);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _OpenFolder(string title, string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string[] _OpenFile(string title, string initialDir, string[] filters, bool multiselect);

	}
}