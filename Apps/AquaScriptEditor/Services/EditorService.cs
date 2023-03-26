using AquaEngine;

namespace AquaEditor
{
	public class EditorService : AquaSystem
	{
		protected override void Enabled()
		{
			Log.Debug("Launched editor service");
			Add<EditorUIService>();
		}

		protected override void Disabled()
		{
			Remove<EditorUIService>();
		}
	}
}