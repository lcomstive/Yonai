using Yonai;
using System;
using Yonai.IO;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace YonaiEditor
{
	/// <summary>
	/// Local settings for the project, not intended to be uploaded or shared with others
	/// </summary>
	public static class LocalProjectSettings
	{
		/// <summary>
		/// Additional settings to be saved and loaded
		/// </summary>
		public static Dictionary<string, JValue> Settings { get; private set; } = new Dictionary<string, JValue>();

		public static Platform BuildTarget
		{
			get
			{
				if(!Settings.ContainsKey("BuildTarget"))
					return BuildTarget = Application.Platform;

				string target = Settings["BuildTarget"].Value<string>();
				if (!string.IsNullOrEmpty(target) && Enum.TryParse(target, out Platform platform))
					return platform;
				else if (!string.IsNullOrEmpty(target))
					Log.Warning($"Failed to parse local project settings build target '{target}', defaulting to host platform '{Application.Platform}'");

				return BuildTarget = Application.Platform;
			}
			set
			{
				Settings["BuildTarget"] = new JValue(Enum.GetName(typeof(Platform), Application.Platform));
				Save();
			}
		}

		private static VFSFile FilePath;

		internal static void SetFilePath(VFSFile path)
		{
			FilePath = path;
			Settings.Clear();

			if (VFS.Exists(path))
				Load();

			if(path.IsValid)
				Log.Debug($"Local project settings path set to '{path}'");
		}

		public static void Load()
		{
			if (!FilePath.IsValid)
				return;

			JObject json = VFS.ReadJSON(FilePath);

			Settings.Clear();
			JProperty[] properties = json.Properties().ToArray();
			try
			{
				foreach (JProperty property in properties)
					Settings.Add(property.Name, property.Value as JValue);
			}
			catch(Exception e) { Log.Exception(e); }
		}

		public static void Save()
		{
			if (!FilePath.IsValid)
				return;

			JObject json = new JObject();

			foreach (var pair in Settings)
				json.Add(pair.Key, pair.Value);

			VFS.WriteJSON(FilePath, json, true);
		}

		public static T Get<T>(string key) => Settings.ContainsKey(key) ? Settings[key].Value<T>() : default;

		public static T Get<T>(string key, T defaultValue) => Settings.ContainsKey(key) ? Settings[key].Value<T>() : defaultValue;

		public static bool TryGet<T>(string key, out T value)
		{
			value = default;
			if (!Settings.ContainsKey(key))
				return false;

			value = Settings[key].Value<T>();
			return true;
		}

		public static void Set<T>(string key, T value) => Settings[key] = new JValue(value);
	}
}