using System.IO;
using System.Collections.Generic;

namespace AquaEngine.IO
{
	public class VFSFileMapping : VFSMapping
	{
		public override FilePermissions Permissions => FilePermissions.ReadWrite;

		/// <returns><paramref name="file"/> as an absolute path on the device's filesystem</returns>
		public override VFSFile ExpandPath(VFSFile file)
		{
			// If mounting is empty, representing an absolute path mounting
			if (string.IsNullOrEmpty(MountPath))
				return file.FullPath;

			// string mountPath = VFS.GetMapping(MountPath, false)?.ExpandPath(MountPath) ?? MountPath;
			string mountPath = VFS.ExpandPath(MountPath) ?? MountPath;
			return mountPath + file.FullPath.Replace(MountPoint, "/");
		}

		public override bool Exists(VFSFile path)
		{
			string fullPath = ExpandPath(path);
			return File.Exists(fullPath) || Directory.Exists(fullPath);
		}

		public override VFSFile[] GetFiles(VFSFile directory, bool recursive = false)
		{
			string absoluteDirectory = ExpandPath(directory);
			List<VFSFile> output = new List<VFSFile>();
			SearchOption searchOption = recursive ? SearchOption.AllDirectories : SearchOption.TopDirectoryOnly;

			// Add directories
			string[] files = Directory.GetDirectories(absoluteDirectory, "*", searchOption);
			for (int i = 0; i < files.Length; i++)
			{
				files[i] = files[i].Replace('\\', '/')
							.Replace(absoluteDirectory, directory);
				output.Add(files[i] + '/');
			}

			// Add files
			files = Directory.GetFiles(absoluteDirectory, "*", searchOption);
			for (int i = 0; i < files.Length; i++)
			{
				files[i] = files[i].Replace('\\', '/')
							.Replace(absoluteDirectory, directory);
				output.Add(files[i]);
			}

			return output.ToArray();
		}

		public override void Copy(VFSFile source, VFSFile destination)
		{
			VFSMapping destinationMapping = VFS.GetMapping(destination, false, FilePermissions.Write);
			if (destinationMapping)
				destinationMapping.Write(destination, Read(source));
		}

		public override void Move(VFSFile source, VFSFile destination)
		{
			VFSMapping destinationMapping = VFS.GetMapping(destination, false, FilePermissions.Write);
			if (!destinationMapping)
			{
				Log.Warning($"Failed to move '{source}' - No mapping found for destination '{destination}'");
				return;
			}

			destinationMapping.Write(destination, Read(source));
			Remove(source);
		}

		/// <returns>Data from file, or empty array if failed</returns>
		public override byte[] Read(VFSFile path) => Exists(path) ? File.ReadAllBytes(ExpandPath(path)) : new byte[0];

		/// <returns>Data from file, or empty string if failed</returns>
		public override string ReadText(VFSFile path) => Exists(path) ? File.ReadAllText(ExpandPath(path)) : string.Empty;

		public override void Remove(VFSFile path) => File.Delete(ExpandPath(path));

		public override bool CreateDirectory(VFSFile target) => Directory.CreateDirectory(ExpandPath(target))?.Exists ?? false;

		public override bool RemoveDirectory(VFSFile target)
		{
			string path = ExpandPath(target);
			if(Directory.Exists(path))
				Directory.Delete(path, true);
			return true;
		}

		public override void Write(VFSFile path, byte[] data) => File.WriteAllBytes(ExpandPath(path), data);

		public override void Write(VFSFile file, string data, bool append = false)
		{
			string path = ExpandPath(file);
			if (append)
				File.AppendAllText(path, data);
			else
				File.WriteAllText(path, data);
		}
	}
}