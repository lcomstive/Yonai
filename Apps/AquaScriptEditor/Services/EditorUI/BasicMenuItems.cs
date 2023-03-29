using AquaEngine;

namespace AquaEditor.EditorUI
{
	internal static class BasicMenuItems
	{
		[MenuItem("File/Reload Scripting")]
		private static void ReloadScripting() => Scripting.Reload();

		[MenuItem("File/Exit", Shortcut = "CTRL + Q")]
		private static void Exit() => Application.Exit();

		[MenuItem("Window/Fullscreen")]
		private static void Fullscreen() =>
			Window.Fullscreen = Window.Fullscreen == FullscreenMode.Windowed ? FullscreenMode.Borderless : FullscreenMode.Windowed;
	}
}