using Yonai;

namespace YonaiEditor.EditorUI
{
	internal static class BasicMenuItems
	{
		[MenuItem("File/Resources/Save")]
		private static void SaveResources() => Resource.SaveDatabase();

		[MenuItem("File/Resources/Load")]
		private static void LoadResources() => Resource.LoadDatabase();

		[MenuItem("File/Reload Scripting")]
		private static void ReloadScripting() => Scripting.Reload();

		[MenuItem("File/Exit", Shortcut = "CTRL + Q")]
		private static void Exit() => Application.Exit();

		[MenuItem("Window/Fullscreen")]
		private static void Fullscreen() =>
			Window.Fullscreen = Window.Fullscreen == FullscreenMode.Windowed ? FullscreenMode.Borderless : FullscreenMode.Windowed;
	}
}