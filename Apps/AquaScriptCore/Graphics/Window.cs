using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Window
	{
		public static Vector2 Resolution
		{
			get
			{
				_aqua_internal_GetResolution(out Vector2 value);
				return value;
			}
			set => _aqua_internal_SetResolution(ref value);
		}

		public static FullscreenMode Fullscreen
		{
			get => (FullscreenMode)_aqua_internal_GetFullscreenMode();
			set => _aqua_internal_SetFullscreenMode((int)value);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_GetResolution(out Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_SetResolution(ref Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _aqua_internal_GetFullscreenMode();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_SetFullscreenMode(int value);
		#endregion
	}
}
