using AquaEngine;
using AquaEngine.Systems;

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
		private EditorState m_State = EditorState.Edit;
		public EditorState State
		{
			get => m_State;
			set => UpdateState(value);
		}

		/// <summary>
		/// IDs of scenes loaded before entering play mode
		/// </summary>
		private UUID[] m_EditModeSceneIDs;

		/// <summary>
		/// IDs of scenes cloned to enter play mode
		/// </summary>
		private World[] m_ClonedWorlds;

		protected override void Destroyed()
		{
			EditorWindow.Destroy();
			EditorWindow.DestroyContext();
		}

		protected override void Enabled()
		{
			try
			{
				Log.Debug("Launched editor service");

				if (!EditorWindow.ContextIsInitialised())
				{
					EditorWindow.InitContext();
					CreateWindow();
				}

				Resource.LoadDatabase();

				Add<ImGUISystem>();
				Add<EditorUIService>();
				Add<BehaviourSystem>();
			}
			catch (System.Exception e) { Log.Exception(e); }
		}

		protected override void Disabled()
		{
			Log.Error("Disabled editor service");

			Remove<EditorUIService>();
			Resource.SaveDatabase();
		}

		protected override void Update()
		{
			if (State == EditorState.Step)
				State = EditorState.Pause;

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
			SceneManager.UnloadAll();

			// Destroy cloned worlds
			foreach (World world in m_ClonedWorlds)
				world.Destroy();

			// Re-load all scenes that were active when entering play mode
			foreach (UUID worldID in m_EditModeSceneIDs)
				SceneManager.Load(World.Get(worldID), SceneAddType.Additive);
		}

		public delegate void OnStateChanged(EditorState oldState, EditorState newState);
		public event OnStateChanged StateChanged;
	}
}