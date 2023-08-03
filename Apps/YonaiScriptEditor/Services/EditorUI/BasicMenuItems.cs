using Yonai;
using YonaiEditor;
using YonaiEditor.Commands;

internal static class _BasicMenuItems
{
	[MenuItem("File/Undo", Shortcut = "CTRL + Z", PrependSeparator = true)]
	public static void Undo() => CommandHistory.Undo();

	[MenuItem("File/Redo", Shortcut = "CTRL + Y")]
	public static void Redo() => CommandHistory.Redo();

	[MenuItem("File/Resources/Save")]
	private static void SaveResources() => Resource.SaveDatabase();

	[MenuItem("File/Resources/Load")]
	private static void LoadResources() => Resource.LoadDatabase();

	[MenuItem("File/Reload Scripting", PrependSeparator = true)]
	private static void ReloadScripting() => Scripting.Reload();

	[MenuItem("File/Exit", Shortcut = "CTRL + Q")]
	private static void Exit() => Application.Exit();

	[MenuItem("Window/Fullscreen")]
	private static void Fullscreen() =>
		Window.Fullscreen = Window.Fullscreen == FullscreenMode.Windowed ? FullscreenMode.Borderless : FullscreenMode.Windowed;
}
