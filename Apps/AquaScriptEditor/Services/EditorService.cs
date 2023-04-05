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
			try
			{
				Log.Debug("Launched editor service");

				if (!EditorWindow.ContextIsInitialised())
				{
					EditorWindow.InitContext();
					CreateWindow();
				}

				Resource.LoadDatabase();
				Add<EditorUIService>();
			}
			catch(System.Exception e) { Log.Exception(e); }
		}

		protected override void Disabled()
		{
			Log.Error("Disabled editor service");

			Remove<EditorUIService>();
			Resource.SaveDatabase();
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
			// EditorWindow.CreationHint(WindowHint.Maximised, true);

			EditorWindow.Create();
		}
	}
}