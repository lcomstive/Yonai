using System.Diagnostics;
using System.IO;

namespace AquaEngine.IO
{
	[DebuggerDisplay("{FullPath}")]
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

		/// <summary>
		/// Checks if a path is set
		/// </summary>
		public bool IsValid => !string.IsNullOrEmpty(FileName);

		public string FullPath => ParentDirectory + FileName;

		public string FileNameWithoutExtension => FileName.Replace(Extension, "");

		public VFSFile(string filepath)
		{
			if (string.IsNullOrEmpty(filepath))
			{
				IsDirectory = false;
				FileName = string.Empty;
				Extension = string.Empty;
				ParentDirectory = string.Empty;
				return;
			}

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

		public override bool Equals(object obj)
		{
			if (!(obj is VFSFile) && !(obj is string))
				return false;
			return ((string)obj).Equals(FullPath);
		}

		public override int GetHashCode() => FullPath.GetHashCode();

		public static implicit operator VFSFile(string path) => new VFSFile(path);
		public static implicit operator string(VFSFile vfsFile) => vfsFile.FullPath;

		public static bool operator ==(VFSFile a, VFSFile b) => a.FullPath.Equals(b.FullPath);
		public static bool operator !=(VFSFile a, VFSFile b) => !a.FullPath.Equals(b.FullPath);
	}
}