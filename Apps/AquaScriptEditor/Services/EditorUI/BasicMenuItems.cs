using AquaEngine;

namespace AquaEditor.EditorUI
{
	internal static class BasicMenuItems
	{
		[MenuItem("File/Reload Scripting", Icon = "Return")]
		private static void ReloadScripting() => Scripting.Reload();

		[MenuItem("File/Exit", Shortcut = "CTRL + Q", Icon = "Power")]
		private static void Exit() => Application.Exit();

		[MenuItem("Window/Fullscreen", Icon = "Larger")]
		private static void Fullscreen() =>
			Window.Fullscreen = Window.Fullscreen == FullscreenMode.Windowed ? FullscreenMode.Borderless : FullscreenMode.Windowed;

		[MenuItem("File/Resources/Save", Icon = "Save")]
		private static void SaveResources() => Resource.SaveDatabase();

		[MenuItem("File/Resources/Load")]
		private static void LoadResources() => Resource.LoadDatabase();
	}
}