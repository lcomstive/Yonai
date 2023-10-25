using Yonai;
using Yonai.IO;
using System.Linq;
using Yonai.Systems;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using YonaiEditor.BuildProcess;
using System.Collections.Generic;

namespace YonaiEditor.Systems
{
	public class ProjectHubService : YonaiSystem
	{
		public static ProjectFile ActiveProject { get; private set; }

		private string HoveredProject = string.Empty;
		private Dictionary<string, ProjectFile> m_Projects = new Dictionary<string, ProjectFile>();
		private const string ProjectCacheFile = "editor://Projects.json";

		protected override void Start()
		{
			Window.Resolution = new IVector2(600, 400);
			EditorWindow.Show();

			LoadProjects();
		}

		protected override void Destroyed()
		{
			EditorWindow.Show(false);
			SaveProjects();
		}

		protected override void Draw()
		{
			ImGUI.Viewport viewport = ImGUI.GetMainViewport();
			Vector2 res = Window.Resolution / Window.ContentScaling;
			ImGUI.SetNextWindowPos(viewport.Position);
			ImGUI.SetNextWindowSize(res);
			ImGUI.Begin("##ProjectHub", ImGUI.WindowFlags.NoDecoration | ImGUI.WindowFlags.NoResize);

			ImGUI.SetCursorPos(res * 0.1f - Vector2.Up * 20);
			ImGUI.Text("Projects");
			ImGUI.SameLine();
			ImGUI.SetCursorPos(new Vector2(res.x * 0.9f - 20, res.y * 0.1f - 25));
			if (ImGUI.Button("+", new Vector2(20, 20)))
			{
				string projectDirectory = FileDialog.OpenFolder("Yonai Project");
				if (!string.IsNullOrEmpty(projectDirectory))
					AddProject(projectDirectory);
			}

			ImGUI.SetNextWindowPos(viewport.Position + res * 0.1f);
			ImGUI.BeginChild("##ProjectList", res * 0.8f, true);

			ProjectFile[] projects = m_Projects.Values.ToArray();
			for (int i = 0; i < projects.Length; i++)
			{
				ProjectFile project = projects[i];

				bool isHovered = HoveredProject.Equals(project.Path);
				if (isHovered)
					ImGUI.PushStyleColour(ImGUI.StyleColour.ChildBg, new Colour(1, 1, 1, 0.1f));

				Vector2 size = new Vector2(0, 50);
				ImGUI.BeginChild($"{project.Name}##{project.Path}", size, true);
				ImGUI.Text(project.Name);
				ImGUI.Text(project.Path.ParentDirectory, Colour.Grey);

				ImGUI.SetCursorPos(ImGUI.ContentRegionAvailable.x - 15, ImGUI.ContentRegionAvailable.y + 12.5f);
				if (ImGUI.Button("X", new Vector2(20, 20)))
					m_Projects.Remove(project.Path);
				ImGUI.EndChild();

				if (isHovered)
					ImGUI.PopStyleColour();

				if (ImGUI.IsItemHovered())
					HoveredProject = project.Path;
				else if (isHovered)
					HoveredProject = string.Empty;

				if (ImGUI.IsItemClicked())
					SelectProject(project);
			}

			ImGUI.EndChild();
			ImGUI.End();
		}

		public static void SelectProject(string directory, bool exitOnFail = true)
		{
			ProjectFile? project = ReadProjectFile(directory);

			if(project.HasValue)
				SelectProject(project.Value);
			else if(exitOnFail)
				Application.Exit();
		}

		public static void SelectProject(ProjectFile project)
		{
			ActiveProject = project;
			Get<ProjectHubService>()?.Enable(false);

			// Add VFS mounts
			VFS.Mount("project://", project.Path.ParentDirectory);
			VFS.Mount("assets://", "project://Assets");

			// Load project resources
			if (!Scripting.IsAssemblyReloading())
			{
				EditorWindow.Show(false);

				// Load project assemblies
				foreach (string assembly in project.Assemblies)
				{
					string vfsPath = VFS.ExpandPath(assembly, true);
					if (VFS.Exists(vfsPath) && !Scripting.IsAssemblyLoaded(vfsPath))
						Scripting.LoadAssembly(vfsPath, true /* Should watch */);
				}
			}

			Resource.LoadDatabase();

			// Add global systems
			Add<EditorUIService>();
			Add<AudioSystem>();

			BehaviourSystem behaviourSystem = Add<BehaviourSystem>();
			if(behaviourSystem)
				behaviourSystem.Enable(false); // Disabled during edit mode
			else
			{
				Log.Warning("Couldn't add behaviour system");
				behaviourSystem = Get<BehaviourSystem>();
			}

			// Initialise game builder
			GameBuilder.Initialise();
		}

		/// <summary>
		/// Fills <see cref="m_Projects"/> from <see cref="ProjectCacheFile"/>
		/// </summary>
		private void LoadProjects()
		{
			m_Projects.Clear();

			if (!VFS.Exists(ProjectCacheFile))
				return;

			JArray json = JsonConvert.DeserializeObject<JArray>(VFS.ReadText(ProjectCacheFile));
			foreach(var item in json)
			{
				string path = item.Value<string>();
				ProjectFile? project = ProjectFile.FromPath(path);
                if (project.HasValue)
					m_Projects.Add(project.Value.Path, project.Value);
			}
		}

		/// <summary>
		/// Saves <see cref="m_Projects"/> to <see cref="ProjectCacheFile"/>
		/// </summary>
		private void SaveProjects()
		{
			JArray json = new JArray();
			foreach (VFSFile path in m_Projects.Keys)
				json.Add(path.FullPath);
			VFS.Write(ProjectCacheFile, JsonConvert.SerializeObject(json));
		}

		private void AddProject(string directory)
		{
			ProjectFile? projectFile = ReadProjectFile(directory);
			if (projectFile.HasValue && !m_Projects.ContainsKey(projectFile.Value.Path))
				m_Projects.Add(projectFile.Value.Path, projectFile.Value);
		}

		private static ProjectFile? ReadProjectFile(string directory)
		{
			directory = directory.Replace('\\', '/');
			string jsonPath = directory + "/Project.json";
			
			if (!VFS.Exists(directory) || !VFS.Exists(jsonPath))
			{
				Log.Warning($"Failed to load '{directory}' - invalid path or project file not found");
				return null;
			}

			return ProjectFile.FromPath(jsonPath);
		}
	}
}