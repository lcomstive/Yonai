using Yonai;
using Yonai.IO;
using Yonai.Graphics;
using System.Collections.Generic;

namespace YonaiEditor
{
	internal static class Icons
	{
		public const string IconDirectory = "assets://Textures/Icons";

		private static Dictionary<string, Texture> s_Icons = new Dictionary<string, Texture>();

		internal static void Load()
		{
			Log.Debug("Loading icons...");

			VFSFile[] files = VFS.GetFiles(IconDirectory, true);
			foreach (VFSFile file in files)
				if (file.Extension.Equals(".png"))
					// Load texture
					s_Icons.Add(file.FileNameWithoutExtension, Resource.Load<Texture>(file.FullPath, new TextureImportSettings() { Filtering = TextureFiltering.Linear }));

			Log.Debug($"Loaded {s_Icons.Count} icons");
		}

		public static Texture Get(string name)
		{
			if (s_Icons.TryGetValue(name, out Texture result))
				return result;
			return null;
		}

		public static bool Has(string name) => s_Icons.ContainsKey(name);
	}
}