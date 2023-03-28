﻿using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Window
	{
		public static IVector2 Resolution
		{
			get
			{
				_GetResolution(out IVector2 value);
				return value;
			}
			set => _SetResolution(ref value);
		}

		public static FullscreenMode Fullscreen
		{
			get => (FullscreenMode)_GetFullscreenMode();
			set => _SetFullscreenMode((int)value);
		}

		public static string Title
		{
			get => _GetTitle();
			set => _SetTitle(value);
		}

		public static bool VSync
		{
			get => _GetVSync();
			set => _SetVSync(value);
		}

		public static bool RequestedToClose => _RequestedToClose();

		/// <summary>
		/// Cycles between fullscreen modes
		/// </summary>
		/// <param name="allowBorderless">
		/// When disabled, skips borderless and goes straight from
		/// windowed to exclusive fullscreen
		/// </param>
		public static void CycleFullscreen(bool allowBorderless = false)
		{
			switch(Fullscreen)
			{
				case FullscreenMode.Windowed:
					Fullscreen = allowBorderless ? FullscreenMode.Borderless : FullscreenMode.Fullscreen;
					break;
				case FullscreenMode.Borderless:
					Fullscreen = FullscreenMode.Fullscreen;
					break;
				case FullscreenMode.Fullscreen:
					Fullscreen = FullscreenMode.Windowed;
					break;
			}
		}

		/// <summary>
		/// Centers the window on the display.
		/// Does nothing if using exclusive fullscreen mode.
		/// </summary>
		public static void CenterOnDisplay() => _CenterOnDisplay();

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetResolution(out IVector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetResolution(ref IVector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetFullscreenMode();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetFullscreenMode(int value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _CenterOnDisplay();

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetTitle();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetTitle(string title);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _RequestedToClose();

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetVSync();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVSync(bool value);
		#endregion
	}
}
