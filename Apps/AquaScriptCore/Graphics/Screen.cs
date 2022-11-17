using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AquaEngine
{
	public enum FullscreenMode : int
	{
		/// <summary>
		/// Shows the application as a normal window
		/// </summary>
		Windowed,

		/// <summary>
		/// The window covers the entire screen, but does not use exclusive fullscreen mode like <see cref="Fullscreen"/>
		/// </summary>
		Borderless,

		/// <summary>
		/// Exclusive fullscreen mode
		/// </summary>
		Fullscreen
	};

	public struct VideoMode
	{
		/// <summary>
		/// Colour depth of the current screen
		/// </summary>
		public int Bits { get; internal set; }
		
		/// <summary>
		/// Resolution of the screen
		/// </summary>
		public Vector2 Resolution { get; internal set; }

		/// <summary>
		/// Maximum frames per second visibly shown to the screen
		/// </summary>
		public int RefreshRate { get; internal set; }
	}

	public static class Screen
	{
		public static VideoMode[] VideoModes
		{
			get
			{
				int modesCount = _aqua_internal_GetVideoModes(out int[] bits, out int[] widths, out int[] heights, out int[] refreshRates);
				VideoMode[] modes = new VideoMode[modesCount];
				for(int i = 0; i < modesCount; i++)
				{
					modes[i].Bits = bits[i];
					modes[i].RefreshRate = refreshRates[i];
					modes[i].Resolution = new Vector2(widths[i], heights[i]);
				}
				return modes;
			}
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _aqua_internal_GetVideoModes(out int[] bits, out int[] widths, out int[] heights, out int[] refreshRates);
		#endregion
	}
}
