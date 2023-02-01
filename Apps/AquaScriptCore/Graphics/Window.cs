using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Window
	{
		public static Vector2 Resolution
		{
			get
			{
				_GetResolution(out Vector2 value);
				return value;
			}
			set => _SetResolution(ref value);
		}

		public static FullscreenMode Fullscreen
		{
			get => (FullscreenMode)_GetFullscreenMode();
			set => _SetFullscreenMode((int)value);
		}

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

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetResolution(out Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetResolution(ref Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetFullscreenMode();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetFullscreenMode(int value);
		#endregion
	}
}
