using System;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;

namespace AquaEngine.IO
{
	[Flags]
	public enum FilePermissions : uint
	{
		None  = 0x00,
		Read  = 0x01,
		Write = 0x02,

		ReadWrite = Read | Write
	}

	public static class VFS
	{
		private static Dictionary<string, List<VFSMapping>> s_Mappings = new Dictionary<string, List<VFSMapping>>();

		public static VFSFile CurrentDirectory => Directory.GetCurrentDirectory() + "/";

		public static bool Exists(VFSFile file) => GetMapping(file)?.Exists(file) ?? false;

		/// <returns><paramref name="file"/> as an absolute path on the device's filesystem</returns>
		public static VFSFile? ExpandPath(VFSFile file, bool needExistingFile = false) => GetMapping(file, needExistingFile)?.ExpandPath(file) ?? null;

		#region Mounting and Mappings
		public static bool HasMount(string mountPoint) => s_Mappings.ContainsKey(mountPoint);

		public static VFSFileMapping Mount(string mountPoint) => Mount<VFSFileMapping>(mountPoint);
		public static VFSFileMapping Mount(string mountPoint, string mountPath) => Mount<VFSFileMapping>(mountPoint, mountPath);

		/// <summary>
		/// Creates an alias using a local directory of the same name
		/// </summary>
		public static T Mount<T>(string mountPoint) where T : VFSMapping, new()
		{
			// This enables absolute paths for the local filesystem
			if (string.IsNullOrEmpty(mountPoint))
				return Mount<T>(string.Empty, string.Empty);

			string mountPath = "./" + mountPoint;
			return Mount<T>(mountPoint, mountPath);
		}

		public static T Mount<T>(string mountPoint, string mountPath) where T : VFSMapping, new()
		{
			// Replace Windows backslashes with single forward slash
			if (!string.IsNullOrEmpty(mountPoint))
				mountPoint = mountPoint.Replace('\\', '/');
			if(!string.IsNullOrEmpty(mountPath))
				mountPath = mountPath.Replace("\\", "/");

			// If no mapping exists yet, create list for it
			if (!s_Mappings.ContainsKey(mountPoint))
				s_Mappings.Add(mountPoint, new List<VFSMapping>());

			T mount = new T
			{
				MountPath = mountPath,
				MountPoint = mountPoint
			};
			s_Mappings[mountPoint].Add(mount);

			Log.Debug($"Mounted '{mountPoint}' to '{mountPath}'");
			return mount;
		}

		public static void Unmount(string mountPoint)
		{
			if (s_Mappings.ContainsKey(mountPoint))
				s_Mappings.Remove(mountPoint);
		}

		public static void Unmount(string mountPoint, string mountPath)
		{
			if (!s_Mappings.ContainsKey(mountPoint))
				return; // No mapping found

			foreach(VFSMapping mapping in s_Mappings[mountPoint])
			{
				if (!mapping.MountPath.Equals(mountPoint))
					continue;

				s_Mappings[mountPoint].Remove(mapping);
				break;
			}
		}

		public static T GetMapping<T>(VFSFile file, bool needExistingFile = true, FilePermissions requiredPermissions = FilePermissions.Read) where T : VFSMapping =>
			(T)GetMapping(file, needExistingFile, requiredPermissions);

		public static VFSMapping GetMapping(VFSFile file, bool needExistingFile = true, FilePermissions requiredPermissions = FilePermissions.Read)
		{
			foreach (string mount in s_Mappings.Keys)
			{
				if (!file.ParentDirectory.Contains(mount))
					continue; // Not in this mount

				foreach (VFSMapping mapping in s_Mappings[mount])
				{
					int flags = (int)(mapping.Permissions & requiredPermissions);
					if (!needExistingFile && (mapping.Permissions & requiredPermissions) != 0)
						return mapping; // Valid mapping found. No file check required
					if (needExistingFile && mapping.Exists(file))
						return mapping;
				}
			}
			return null;
		}
		#endregion

		#region Read and Write
		public static byte[] Read(VFSFile path)
		{
			if(!path.IsValid) return new byte[0];
			return GetMapping(path)?.Read(path) ?? new byte[0];
		}

		public static string ReadText(VFSFile path)
		{
			if (!path.IsValid) return string.Empty;
			return GetMapping(path)?.ReadText(path) ?? string.Empty;
		}

		public static void Write(VFSFile path, byte[] data, bool append = false) => GetMapping(path, needExistingFile: false)?.Write(path, data);
		public static void Write(VFSFile path, string contents, bool append = false) => GetMapping(path, needExistingFile: false)?.Write(path, contents, append);

		/// <summary>
		/// Converts <paramref name="target"/> to JSON using <see cref="ISerializable.OnSerialize"/> and writes contents to <paramref name="path"/>
		/// </summary>
		public static void Write(VFSFile path, ISerializable target) => WriteJSON(path, target.OnSerialize());

		/// <summary>
		/// Reads <paramref name="path"/> as JSON and converts to type <typeparamref name="T"/>
		/// </summary>
		/// <param name="path">JSON file path</param>
		/// <returns>New instance of <typeparamref name="T"/> with <see cref="ISerializable.OnDeserialize(JObject)"/> called using contents of file</returns>
		public static T Read<T>(VFSFile path) where T : ISerializable, new()
		{
			T instance = new T();
			instance.OnDeserialize(ReadJSON(path));
			return instance;
		}
		
		/// <summary>
		/// Converts <paramref name="target"/> to JSON using <see cref="ISerializable.OnSerialize"/> and writes contents to <paramref name="path"/>
		/// </summary>
		public static void WriteJSON(VFSFile path, JObject contents, bool indent = true) => Write(path, JsonConvert.SerializeObject(contents, indent ? Formatting.Indented : Formatting.None));

		/// <summary>
		/// Reads <paramref name="path"/> as JSON
		/// </summary>
		/// <param name="path">JSON file path</param>
		/// <returns><see cref="JObject"/> with contents of file</returns>
		public static JObject ReadJSON(VFSFile path) => JsonConvert.DeserializeObject<JObject>(ReadText(path));
		#endregion

		#region File Operations
		public static void Remove(VFSFile path) => GetMapping(path, true, FilePermissions.Write)?.Remove(path);
		public static void Copy(VFSFile source, VFSFile destination) => GetMapping(source, true)?.Copy(source, destination);
		public static void Move(VFSFile source, VFSFile destination) => GetMapping(source, true)?.Move(source, destination);

		public static bool CreateDirectory(VFSFile path) => GetMapping(path)?.CreateDirectory(path) ?? false;
		public static bool RemoveDirectory(VFSFile path) => GetMapping(path)?.RemoveDirectory(path) ?? false;

		public static VFSFile[] GetFiles(VFSFile directory, bool recursive = false) => GetMapping(directory)?.GetFiles(directory, recursive) ?? new VFSFile[0];
		#endregion
	}
}