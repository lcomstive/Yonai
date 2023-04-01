using AquaEngine;

namespace AquaEditor
{
	public class EditorService : AquaSystem
	{
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

			Add<EditorUIService>();
		}

		protected override void Disabled()
		{
			Log.Error("Disabled editor service");

			Remove<EditorUIService>();
		}

		protected override void Update()
		{
			if (Window.RequestedToClose)
			{
				EditorWindow.Destroy();
				Application.Exit();
			}
		}

		private void CreateWindow()
		{
			EditorWindow.CreationHint(WindowHint.Maximised, true);

			EditorWindow.Create();
		}
	}
}