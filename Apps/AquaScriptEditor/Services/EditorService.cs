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

			m_State = state;
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
		}

		private void EnterPlayMode()
		{
			World[] activeScenes = SceneManager.GetActiveScenes();

			m_EditModeSceneIDs = new UUID[activeScenes.Length];
			for (int i = 0; i < activeScenes.Length; i++)
				m_EditModeSceneIDs[i] = activeScenes[i].ID;
		}

		private void ExitPlayMode()
		{
			SceneManager.UnloadAll();

			foreach (UUID worldID in m_EditModeSceneIDs)
				SceneManager.Load(World.Get(worldID), SceneAddType.Additive);
		}
	}
}