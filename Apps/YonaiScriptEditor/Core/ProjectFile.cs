using System;
using System.Linq;
using Yonai;
using Yonai.IO;
using Newtonsoft.Json.Linq;

namespace YonaiEditor
{
	public struct ProjectFile : ISerializable
	{
		public VFSFile Path { get; set; }

		public string Name { get; set; }

		public VFSFile AssetDirectory { get; set; }

		/// <summary>
		/// Scene to first load when launching built game
		/// </summary>
		public VFSFile InitialScene { get; set; }

		public string[] Assemblies { get; set; }

		public string[] EditorAssemblies { get; set; }

		public ProjectFile(string name = "Yonai Project")
		{
			Name = name;
			Path = "project://project.json";
			AssetDirectory = "Assets";
			InitialScene = string.Empty;
			Assemblies = new string[0];
			EditorAssemblies = new string[0];
		}

		public static ProjectFile? FromPath(VFSFile path)
		{
			if (!VFS.Exists(path))
				return null;
			try
			{
				ProjectFile project = VFS.Read<ProjectFile>(path);
				project.Path = path;
				return project;
			}
			catch(Exception e)
			{
				Log.Exception(e, $"Failed to read project '{path}'");
				return null;
			}
		}

		public void OnDeserialize(JObject json)
		{
			Name = json["Name"]?.Value<string>() ?? "Project Name";
			InitialScene = (VFSFile)(json["InitialScene"]?.Value<string>() ?? string.Empty);
			AssetDirectory = (VFSFile)(json["AssetDir"]?.Value<string>() ?? "Assets");
			Assemblies = json["Assemblies"]?.Values<string>().ToArray() ?? new string[0];
			EditorAssemblies = json["EditorAssemblies"]?.Values<string>().ToArray() ?? new string[0];
		}

		public JObject OnSerialize() => new JObject(
				new JProperty("Name", Name),
				new JProperty("AssetDir", AssetDirectory.FullPath),
				new JProperty("InitialScene", InitialScene.FullPath),
				new JProperty("Assemblies", new JArray(Assemblies)),
				new JProperty("EditorAssemblies", new JArray(EditorAssemblies))
			);
	}
}