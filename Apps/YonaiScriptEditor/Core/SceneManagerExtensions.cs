using Yonai;
using YonaiEditor.Commands;
using YonaiEditor.Systems;
using YonaiEditor.Views;

namespace YonaiEditor
{
	public static class SceneManagerHelper
	{
		[MenuItem("File/Scene/Save", Shortcut = "CTRL + S")]
		public static void SaveScenes()
		{
			World[] scenes = SceneManager.GetActiveScenes();
			foreach (World scene in scenes)
				Resource.SaveToDisk(scene);
		}

		[MenuItem("File/Scene/Reload")]
		public static void ReloadScenes()
		{
			Entity inspectorTarget = InspectorView.Target as Entity;

			World[] scenes = SceneManager.GetActiveScenes();
			foreach (World scene in scenes)
			{
				Resource.LoadFromDisk(scene);

				// Update inspector target to use new entity, as they get recreated
				if (inspectorTarget?.World?.ID == scene.ID)
					InspectorView.Target = scene.GetEntity(inspectorTarget.ID);
			}

			CommandHistory.Clear();
		}
	}
}