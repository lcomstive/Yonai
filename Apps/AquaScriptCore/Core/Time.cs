using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Time
	{
		public static float FPS				=> _FPS();
		public static float DeltaTime		=> _DeltaTime();
		public static float TimeSinceLaunch => _TimeSinceLaunch();

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _FPS();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _DeltaTime();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _TimeSinceLaunch();
		#endregion
	}
}