using AquaEngine;
using System.Runtime.CompilerServices;

namespace AquaEditor
{
	internal enum WindowHint : int
	{
		/// <summary>
		/// Specifies whether the windowed mode window will be given input focus when created.<br></br><br></br>
		/// 
		/// Value is boolean. This hint is ignored for full screen and initially hidden windows.
		/// </summary>
		Focused = 0x00020001,

		/// <summary>
		/// Indicates whether the specified window is resizable by the user.
		/// <see cref=""/>
		/// </summary>
		Resizable = 0x00020003,

		/// <summary>
		/// Specifies whether the windowed mode window will be initially visible.<br></br><br></br>
		/// 
		/// Value is boolean. This hint is ignored for full screen windows.
		/// </summary>
		Visible = 0x00020004,

		/// <summary>
		/// Specifies whether the windowed mode window will have window decorations such as a border,
		/// a close widget, etc.
		/// An undecorated window will not be resizable by the user but will still allow the user to
		/// generate close events on some platforms.<br></br><br></br>
		/// 
		/// Value is boolean. This hint is ignored for full screen windows.
		/// </summary>
		Decorated = 0x00020005,

		/// <summary>
		/// Specifies whether the full screen window will automatically iconify and restore the previous video mode on input focus loss.<br></br><br></br>
		/// 
		/// Value is booleean. This hint is ignored for windowed mode windows.
		/// </summary>
		AutoIconify = 0x00020006,

		/// <summary>
		/// Specifies whether the windowed mode window will be floating above other regular windows,
		/// also called topmost or always-on-top. This is intended primarily for debugging purposes and cannot
		/// be used to implement proper full screen windows.<br></br><br></br>
		/// 
		/// Value is boolean. This hint is ignored for full screen windows.
		/// </summary>
		Floating = 0x00020007,

		/// <summary>
		/// Specifies whether the windowed mode window will be maximized when created.<br></br><br></br>
		/// 
		/// Value is boolean. This hint is ignored for full screen windows.
		/// </summary>
		Maximised = 0x00020008,

		/// <summary>
		/// Specifies whether the window framebuffer will be transparent.<br></br>
		/// If enabled and supported by the system, the window framebuffer alpha channel will
		/// be used to combine the framebuffer with the background.<br></br>
		/// This does not affect window decorations.<br></br><br></br>
		/// 
		/// Value is boolean.
		/// </summary>
		TransparentFramebuffer = 0x0002000A
	}

	/// <summary>
	/// Extension of the <see cref="Window"/> class
	/// </summary>
	public static class EditorWindow
	{
		internal static void CreationHint(WindowHint hint, int value) => CreationHintInt((int)hint, value);
		internal static void CreationHint(WindowHint hint, bool value) => CreationHintBool((int)hint, value);

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void InitContext();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void DestroyContext();

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void Create();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void Destroy();

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void ResetHints();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void CreationHintInt(int flag, int value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void CreationHintBool(int flag, bool value);
	}
}