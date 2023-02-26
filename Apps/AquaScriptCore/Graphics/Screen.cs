using System.Runtime.CompilerServices;

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
		/// Resolution of the screen
		/// </summary>
		public IVector2 Resolution { get; internal set; }

		/// <summary>
		/// Maximum frames per second visibly shown to the screen
		/// </summary>
		public float RefreshRate { get; internal set; }
	}

	public static class Screen
	{
		public static VideoMode VideoMode
		{
			get
			{
				_GetVideoMode(out IVector2 resolution, out float refreshRate);
				return new VideoMode
				{
					Resolution = resolution,
					RefreshRate = refreshRate
				};
			}
			set
			{
				IVector2 resolution = value.Resolution;
				_SetVideoMode(ref resolution, value.RefreshRate);
			}
		}

		public static VideoMode[] VideoModes
		{
			get
			{
				uint modesCount = _GetVideoModes(out int[] widths, out int[] heights, out float[] refreshRates);
				VideoMode[] modes = new VideoMode[modesCount];
				for(int i = 0; i < modesCount; i++)
				{
					modes[i].RefreshRate = refreshRates[i];
					modes[i].Resolution = new IVector2(widths[i], heights[i]);
				}
				return modes;
			}
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetVideoMode(out IVector2 resolution, out float refreshRate);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetVideoModes(out int[] widths, out int[] heights, out float[] refreshRates);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVideoMode(ref IVector2 resolution, float refreshRate);
		#endregion
	}
}
