using System;
using System.IO;
using AquaEngine;
using AquaEngine.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace AquaEditor.BuildProcess
{
	public class WindowsBuildProcess : IBuildProcess
	{
		public bool OutputIsFolder => true;

		public void Execute(string outputFolder, ProjectFile project)
		{
			if(string.IsNullOrEmpty(outputFolder))
			{
				outputFolder = FileDialog.OpenFolder("Build Output");
				if(string.IsNullOrEmpty(outputFolder))
				{
					Log.Error("Cannot build - not output folder selected");
					return;
				}
			}
			outputFolder = outputFolder.Replace('\\', '/');

			VFS.Mount("build://", outputFolder);

			// Launching executable
			VFS.Copy("app://BaseGame.exe", $"build://{project.Name}.exe");

			// Mono assemblies
			// VFS.CreateDirectory("build://Assets/Mono");
			// VFS.Copy("mono://", "build://Assets/Mono/");

			// Resources file
			// Read in resource file as JSON, write as JSON without indentation/whitespace
			JArray resources = JsonConvert.DeserializeObject<JArray>(VFS.ReadText("project://Resources.json"));
			VFS.Write("build://Assets/Resources.json", JsonConvert.SerializeObject(resources, Formatting.None));

			// Project assemblies
			VFS.CreateDirectory("build://Assets/Assemblies");
			foreach (string assemblyPath in project.Assemblies)
				VFS.Copy(assemblyPath, "build://Assets/Assemblies/" + Path.GetFileName(assemblyPath));

			// Copy editor default assets
			VFS.Copy("app://Assets/", "build://Assets/Editor/");

			// Project files
			VFS.CreateDirectory("build://Assets/ProjectFiles");
			VFS.Copy("project://Assets/", "build://Assets/ProjectFiles");

			// If base game was compiled as shared library, will need to copy dependency .dll files
			if (VFS.Exists("app://AquaEngine.dll") || VFS.Exists("app://AquaEngined.dll"))
			{
				VFSFile[] localFiles = VFS.GetFiles("app://");
				foreach (var file in localFiles)
					if (file.Extension == ".dll" && file.FileName != "AquaScriptCore.dll" && file.FileName != "AquaScriptEditor.dll")
						VFS.Copy(file, "build://");
			}

			CreateLaunchJSON(project);

			// Unmount virtual mount
			VFS.Unmount("build://");
		}

		private void CreateLaunchJSON(ProjectFile project)
		{
			JObject json = new JObject();

			// Assemblies
			JArray assemblies = new JArray();
			foreach(string assembly in project.Assemblies)
				assemblies.Add(new VFSFile(assembly).FileName);
			json["Assemblies"] = assemblies;

			// Systems
			JArray systems = new JArray();
			/*
			AquaSystem[] globalSystems = AquaSystem.GetAll();
			foreach (AquaSystem system in globalSystems)
			{
				if (system == null)
					continue;

				Type type = system.GetType();
				systems.Add($"{type.FullName}, {type.Assembly.GetName().Name}");
			}
			*/
			json["GlobalSystems"] = systems;

			// Scenes
			// NOTE: Currently just adds all .json files in project://Scenes
			// TODO: Give user ability to re-order scenes and exclude scenes
			JArray scenes = new JArray();
			VFSFile[] sceneFiles = VFS.GetFiles("project://Assets/Scenes", true /* Recurse */);
			foreach (VFSFile sceneFile in sceneFiles)
				if(sceneFile.Extension == ".json")
					scenes.Add(sceneFile.FullPath);
			json["Scenes"] = scenes;

			// Save to output folder
			VFS.WriteJSON("build://Assets/Launch.json", json, false /* Indent */);
		}
	}
}