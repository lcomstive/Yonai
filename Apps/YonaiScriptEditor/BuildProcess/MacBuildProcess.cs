using Yonai;
using Yonai.IO;
using System.IO;
using Newtonsoft.Json;
using YonaiEditor.Systems;
using Newtonsoft.Json.Linq;

namespace YonaiEditor.BuildProcess
{
	public class MacBuildProcess : IBuildProcess
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

			VFSMapping mapping = VFS.Mount("buildApp://", $"{outputFolder}/{project.Name}.app");
			mapping.CreateDirectory("buildApp://Contents/");
			mapping.CreateDirectory("buildApp://Contents/MacOS/");
			mapping.CreateDirectory("buildApp://Contents/Resources/");

			// Launching executable
			// Copy explicitly using the native file API to preserve file permissions
			File.Copy(VFS.ExpandPath("app://BaseGame"), mapping.ExpandPath($"buildApp://Contents/MacOS/{project.Name}"), overwrite: true);

			// Info.plist
			VFS.Write($"buildApp://Contents/Info.plist", CreatePLIST(project));

			VFS.Mount("build://", $"buildApp://Contents/Resources");
			
			// Default icon
			VFS.Copy("app://AppIcon.icns", "build://AppIcon.icns");

			// Resources file
			// Read in resource file as JSON, write as JSON without indentation/whitespace
			JArray resources = JsonConvert.DeserializeObject<JArray>(VFS.ReadText("project://Resources.json"));
			VFS.Write("build://Assets/Resources.json", JsonConvert.SerializeObject(resources, Formatting.None));

			// Project assemblies
			VFS.CreateDirectory("build://Assets/Assemblies");
			foreach(string assemblyPath in project.Assemblies)
				VFS.Copy(assemblyPath, "build://Assets/Assemblies/" + Path.GetFileName(assemblyPath));

			// Copy editor default assets
			VFS.Copy("app://Assets/", "build://Assets/Editor/");

			// Project files
			VFS.CreateDirectory("build://Assets/ProjectFiles");
			VFS.Copy("project://Assets/", "build://Assets/ProjectFiles");

			// Copy all required .dylib files
			VFSFile[] localFiles = VFS.GetFiles("app://", recursive: false);
			foreach(var file in localFiles)
				if(file.Extension == ".dylib")
					VFS.Copy(file, "build://");

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
			YonaiSystem[] globalSystems = YonaiSystem.GetAll();
			foreach (YonaiSystem system in globalSystems)
			{
				if (system == null)
					continue;

				Type type = system.GetType();
				systems.Add($"{type.FullName}, {type.Assembly.GetName().Name}");
			}
			*/
			json["GlobalSystems"] = systems;

			// Scene to load on startup
			json["InitialScene"] = ProjectHubService.ActiveProject.InitialScene.FullPath;

			// Save to output folder
			VFS.WriteJSON("build://Assets/Launch.json", json, false /* Indent */);
		}

		const string PLISTTemplate = @"
			<?xml version=""1.0"" encoding=""UTF-8""?>
			<!DOCTYPE plist PUBLIC ""-//Apple Computer//DTD PLIST 1.0//EN"" ""http://www.apple.com/DTDs/PropertyList-1.0.dtd"">
			<plist version=""1.0"">
			<dict>
				<key>CFBundleDevelopmentRegion</key>
				<string>English</string>
				<key>CFBundleExecutable</key>
				<string>${PROJECT_NAME}</string>
				<key>CFBundleGetInfoString</key>
				<string>${PROJECT_INFO}</string>
				<key>CFBundleIconFile</key>
				<string>AppIcon.icns</string>
				<key>CFBundleIdentifier</key>
				<string>${PROJECT_NAME}</string>
				<key>CFBundleInfoDictionaryVersion</key>
				<string>6.0</string>
				<key>CFBundleLongVersionString</key>
				<string>${PROJECT_VERSION}</string>
				<key>CFBundleName</key>
				<string>${PROJECT_NAME}</string>
				<key>CFBundlePackageType</key>
				<string>APPL</string>
				<key>CFBundleShortVersionString</key>
				<string>${PROJECT_VERSION}</string>
				<key>CFBundleSignature</key>
				<string>com.${PROJECT_NAME}</string>
				<key>CFBundleVersion</key>
				<string>${PROJECT_VERSION}</string>
				<key>CSResourcesFileMapped</key>
				<true/>
				<key>NSHumanReadableCopyright</key>
				<string>${PROJECT_NAME} (c) 2023</string>
				<key>NSHighResolutionCapable</key>
				<string>True</string>
			</dict>
			</plist>
			";

		private string CreatePLIST(ProjectFile project) =>
			PLISTTemplate
				.Replace("${PROJECT_NAME}", project.Name)
				.Replace("${PROJECT_VERSION}", "1.0.0");
	}
}