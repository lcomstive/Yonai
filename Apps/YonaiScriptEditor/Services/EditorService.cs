using Yonai;
using Yonai.IO;
using Yonai.Systems;
using YonaiEditor.Systems;
using YonaiEditor.BuildProcess;

namespace YonaiEditor
{
	public enum EditorState
	{
		Edit,
		Play,
		Pause,
		Step
	}

	public class EditorService : YonaiSystem
	{
		private EditorState m_State = EditorState.Edit;
		public EditorState State
		{
			get => m_State;
			set => UpdateState(value);
		}

		/// <summary>
		/// IDs of scenes loaded before entering play mode
		/// </summary>
		private UUID[] m_EditModeSceneIDs = new UUID[0];

		/// <summary>
		/// IDs of scenes cloned to enter play mode
		/// </summary>
		private World[] m_ClonedWorlds = new World[0];

		private const string ProjectFilePath = "project://project.json";
		private const string ImGUIFile = "editor://EditorLayout.ini";

		protected override void Enabled()
		{
			Log.Debug("Launched editor service");

			InitialiseVFS();

			if (Application.HasArg("build"))
			{
				DoBuildAndExit();
				return;
			}

			string projectPath = VFS.HasMount("project://") ? (VFS.ExpandPath("project://")?.FullPath as string) : Application.GetArg("projectpath", null);
			Log.Debug($"Project path: {projectPath}");

			if (!Scripting.IsAssemblyReloading())
			{
				EditorWindow.InitContext();
				EditorWindow.CreationHint(WindowHint.Visible, false); // Hide by default
				EditorWindow.Create();
			}

			InitImGUI();

			if (!string.IsNullOrEmpty(projectPath))
				ProjectHubService.SelectProject(projectPath);
			else
				Add<ProjectHubService>();
		}

		protected override void Disabled()
		{
			if (Application.HasArg("build"))
				return; // Nothing below was loaded as we just built a project

			if (!Scripting.IsAssemblyReloading())
			{
				EditorWindow.Destroy();
				EditorWindow.DestroyContext();
			}

			Log.Error("Disabled editor service");

			Remove<EditorUIService>();
			Remove<BehaviourSystem>();

			Resource.SaveDatabase();

			VFS.Unmount("app://");
			VFS.Unmount("assets://");
			VFS.Unmount("editor://");
		}

		private void InitialiseVFS()
		{
			VFS.AllowAbsolutePaths = true;
			
			VFS.Mount("app://", Application.ExecutableDirectory);
			VFS.Mount("assets://", "app://Assets"); // Editor assets
			VFS.Mount("editor://", Application.PersistentDirectory);
		}

		private void InitImGUI()
		{
			// Settings save/load location
			ImGUI.SetIniFilename(VFS.ExpandPath(ImGUIFile));

			Add<ImGUISystem>()?.Enable(true);

			// Window set initial content scale and listen for change
			OnWindowContentScaleChanged(Window.ContentScaling);
			Window.ContentScaleChanged += OnWindowContentScaleChanged;
		}

		private void OnWindowContentScaleChanged(Vector2 resolution)
		{
			ImGUI.SetDisplayFramebufferScale(Window.ContentScaling);
			ImGUI.SetFontGlobalScale(Window.ContentScaling.x);
		}

		protected override void Update()
		{
			if (State == EditorState.Step)
				State = EditorState.Pause;

			// Toggle play mode with CTRL+P
			if (ImGUI.IsKeyDown(Key.LeftControl) && ImGUI.IsKeyPressed(Key.P))
				State = State == EditorState.Edit ? EditorState.Play : EditorState.Edit;

			if (Window.RequestedToClose)
			{
				EditorWindow.Destroy();
				Application.Exit();
			}
		}

		private void UpdateState(EditorState state)
		{
			if (state == m_State) return; // No change

			// If in edit mode, switch to play mode
			if (State == EditorState.Edit)
				EnterPlayMode();
			// If already in play mode and requesting edit mode
			else if (state == EditorState.Edit)
				ExitPlayMode();

			switch (state)
			{
				default:
				case EditorState.Edit:
					Get<BehaviourSystem>().Enable(false);
					break;
				case EditorState.Pause:
					Get<BehaviourSystem>().Enable(false);
					break;
				case EditorState.Step:
				case EditorState.Play:
					Get<BehaviourSystem>().Enable(true);
					break;
			}

			StateChanged?.Invoke(m_State, m_State = state);
		}

		private void EnterPlayMode()
		{
			Log.Trace("Entering play mode");

			World[] activeScenes = SceneManager.GetActiveScenes();
			SceneManager.UnloadAll();

			m_EditModeSceneIDs = new UUID[activeScenes.Length];
			m_ClonedWorlds = new World[activeScenes.Length];
			for (int i = 0; i < activeScenes.Length; i++)
			{
				m_EditModeSceneIDs[i] = activeScenes[i].ID;
				m_ClonedWorlds[i] = activeScenes[i].Clone();
				m_ClonedWorlds[i].Name += " (Clone)";
				SceneManager.Load(m_ClonedWorlds[i], SceneAddType.Additive);
			}
		}

		private void ExitPlayMode()
		{
			Log.Trace("Exiting play mode");

			SceneManager.UnloadAll();

			// Destroy cloned worlds
			foreach (World world in m_ClonedWorlds)
				world?.Destroy();

			// Re-load all scenes that were active when entering play mode
			foreach (UUID worldID in m_EditModeSceneIDs)
				SceneManager.Load(World.Get(worldID), SceneAddType.Additive, false);

			// Force mouse state to be normal when not in play mode
			Input.MouseState = MouseState.Normal;
		}

		private void DoBuildAndExit()
		{
			string projectPath = Application.GetArg("projectpath");
			if (string.IsNullOrEmpty(projectPath))
			{
				Log.Error("Cannot load project - no ProjectPath argument set");
				return;
			}

			VFS.Mount("project://", projectPath);
			VFS.Mount("assets://", "project://Assets");

			// Read project file
			ProjectFile project = VFS.Read<ProjectFile>(ProjectFilePath);

			string output = Application.GetArg("output");
			if(!string.IsNullOrEmpty(output))
				Log.Info($"Build output folder: {output}");

			try
			{
				if (GameBuilder.Initialise())
					GameBuilder.StartBuild(project, output);
			}
			catch(System.Exception e) { Log.Exception(e); }

			Application.Exit();
		}

		public delegate void OnStateChanged(EditorState oldState, EditorState newState);
		public event OnStateChanged StateChanged;
	}
}