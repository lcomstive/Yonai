using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using System.Collections.Generic;
using System.Linq;

namespace Yonai.IO
{
	public abstract class VFSMapping
	{
		public virtual FilePermissions Permissions { get; }

		public string MountPoint { get; internal set; } = string.Empty;
		public string MountPath  { get; internal set; } = string.Empty;

		public abstract bool Exists(VFSFile path);

		/// <returns><paramref name="file"/> as an absolute path on the device's filesystem</returns>
		public abstract VFSFile ExpandPath(VFSFile file, bool needExistingFile = false);

		public abstract byte[] Read(VFSFile path);
		public abstract string ReadText(VFSFile path);

		/// <summary>
		/// Reads <paramref name="path"/> as JSON
		/// </summary>
		/// <param name="path">JSON file path</param>
		/// <returns><see cref="JObject"/> with contents of file</returns>
		public JObject ReadJSON(VFSFile path) => JsonConvert.DeserializeObject<JObject>(ReadText(path));

		/// <summary>
		/// Reads <paramref name="path"/> as JSON and converts to type <typeparamref name="T"/>
		/// </summary>
		/// <param name="path">JSON file path</param>
		/// <returns>New instance of <typeparamref name="T"/> with <see cref="ISerializable.OnDeserialize(JObject)"/> called using contents of file</returns>
		public T Read<T>(VFSFile path) where T : ISerializable, new()
		{
			T instance = new T();
			instance.OnDeserialize(ReadJSON(path));
			return instance;
		}

		public abstract void Write(VFSFile path, byte[] data);
		public abstract void Write(VFSFile path, string data, bool append = false);

		/// <summary>
		/// Converts <paramref name="target"/> to JSON using <see cref="ISerializable.OnSerialize"/> and writes contents to <paramref name="path"/>
		/// </summary>
		public void WriteJSON(VFSFile path, JObject contents, bool indent = true) => Write(path, JsonConvert.SerializeObject(contents, indent ? Formatting.Indented : Formatting.None));

		/// <summary>
		/// Converts <paramref name="target"/> to JSON using <see cref="ISerializable.OnSerialize"/> and writes contents to <paramref name="path"/>
		/// </summary>
		public void Write(VFSFile path, ISerializable target) => WriteJSON(path, target.OnSerialize());

		public abstract void Remove(VFSFile path);
		public abstract void Copy(VFSFile source, VFSFile destination);
		public abstract void Move(VFSFile source, VFSFile destination);

		public abstract bool CreateDirectory(VFSFile target);
		public abstract bool RemoveDirectory(VFSFile target);

		public abstract VFSFile[] GetFiles(VFSFile directory, bool recursive = false);

		public List<VFSFile> GetFilesByExtension(params string[] extensions)
		{
			List<VFSFile> output = new List<VFSFile>();
			VFSFile[] files = GetFiles("/", true);

			string mountPoint = MountPoint.Substring(0, MountPoint.Length - 1);
			foreach (VFSFile file in files)
				if (extensions.Contains(file.Extension))
					output.Add(mountPoint + file);

			return output;
		}

		public static implicit operator bool(VFSMapping x) => x != null;
	}
}