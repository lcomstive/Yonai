using AquaEngine;
using AquaEngine.IO;
using AquaEngine.Systems;
using AquaEditor.Systems;
using AquaEditor.BuildProcess;

namespace AquaEditor
{
	public enum EditorState
	{
		Edit,
		Play,
		Pause,
		Step
	}

	public class EditorService : AquaSystem
	{
		public ProjectFile Project { get; private set; }

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

		protected override void Destroyed()
		{
			EditorWindow.Destroy();
			EditorWindow.DestroyContext();
		}

		protected override void Enabled()
		{
			Log.Debug("Launched editor service");

			if (!EditorWindow.ContextIsInitialised())
			{
				EditorWindow.InitContext();
				CreateWindow();
			}

			Resource.LoadDatabase();

			// Add base global systems
			Add<ImGUISystem>();
			Add<BehaviourSystem>().Enable(false); // Disabled during edit mode

			// Set ImGUI layout file
			#region ImGUI Setup
			// Settings from disk
			ImGUI.SetIniFilename(ImGUIFile);
			ImGUI.LoadIniSettingsFromDisk(ImGUIFile);

			// Window set initial content scale and listen for change
			OnWindowContentScaleChanged(Window.ContentScaling);
			Window.ContentScaleChanged += OnWindowContentScaleChanged;
			#endregion

			// Read project file
			Project = VFS.Read<ProjectFile>(ProjectFilePath);
			foreach (string assembly in Project.Assemblies)
				if (VFS.Exists(assembly) && !Scripting.IsAssemblyLoaded(assembly))
					Scripting.LoadAssembly(assembly, true /* Should watch */);

			// Launch editor UI
			Add<EditorUIService>();

			GameBuilder.Initialise();
		}

		private void OnWindowContentScaleChanged(Vector2 resolution)
		{
			ImGUI.SetDisplayFramebufferScale(Window.ContentScaling);
			ImGUI.SetFontGlobalScale(Window.ContentScaling.x);
		}

		protected override void Disabled()
		{
			Log.Error("Disabled editor service");

			Remove<EditorUIService>();
			Remove<ImGUISystem>();
			Remove<BehaviourSystem>();

			Resource.SaveDatabase();
			ImGUI.SaveIniSettingsToDisk(ImGUIFile);
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

		private void CreateWindow()
		{
			// EditorWindow.CreationHint(WindowHint.Maximised, true);
			EditorWindow.Create();
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

		public delegate void OnStateChanged(EditorState oldState, EditorState newState);
		public event OnStateChanged StateChanged;
	}
}