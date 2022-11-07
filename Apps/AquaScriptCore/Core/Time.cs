using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Time
	{
		public static float FPS				=> _aqua_internal_GetFPS();
		public static float DeltaTime		=> _aqua_internal_GetDeltaTime();
		public static float TimeSinceLaunch => _aqua_internal_TimeSinceLaunch();

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_GetFPS();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_GetDeltaTime();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_TimeSinceLaunch();
		#endregion
	}
}