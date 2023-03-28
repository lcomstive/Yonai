using AquaEngine;

namespace AquaEditor
{
	public class EditorService : AquaSystem
	{
		protected override void Enabled()
		{
			Log.Debug("Launched editor service");

			EditorWindow.InitContext();
			EditorWindow.Create();

			Add<EditorUIService>();
		}

		protected override void Update()
		{
			if (Window.RequestedToClose)
			{
				EditorWindow.Destroy();
				Application.Exit();
			}
		}

		protected override void Disabled()
		{
			Remove<EditorUIService>();

			EditorWindow.Destroy();
			EditorWindow.DestroyContext();
		}
	}
}