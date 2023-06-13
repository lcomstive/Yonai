using AquaEngine;
using AquaEngine.IO;
using AquaEngine.Graphics;
using System.Collections.Generic;

namespace AquaEditor
{
	public static class Icons
	{
		public const string IconDirectory = "assets://Textures/Icons";

		public const string SavePath = "assets://Textures/Icons/Kenney/save.png";

		private static Dictionary<int, Texture> s_Icons = new Dictionary<int, Texture>();

		internal static void Load()
		{
			Log.Debug("Loading icons...");

			VFSFile[] files = VFS.GetFiles(IconDirectory, true);
			foreach (VFSFile file in files)
			{
				if (!file.Extension.Equals(".png"))
					continue;

				// Load texture
				Add(
					file.FileName.Replace(file.Extension, ""),
					Resource.Load<Texture>(file.FullPath, new TextureImportSettings() { Filtering = TextureFiltering.Linear })
				);
			}

			Log.Debug($"Loaded {s_Icons.Count} icons");
		}

		public static Texture Get(string name) => string.IsNullOrEmpty(name) ? null : Get(name.GetHashCode());
		public static Texture Get(int hashCode) => s_Icons.ContainsKey(hashCode) ? s_Icons[hashCode] : null;

		public static void Add(string name, Texture texture)
		{
			int hashCode = name.GetHashCode();
			if (!s_Icons.ContainsKey(hashCode))
				s_Icons.Add(hashCode, texture);
		}
	}
}