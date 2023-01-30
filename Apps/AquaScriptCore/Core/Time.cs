using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Time
	{
		public static float FPS				=> _aqua_internal_FPS();
		public static float DeltaTime		=> _aqua_internal_DeltaTime();
		public static float TimeSinceLaunch => _aqua_internal_TimeSinceLaunch();

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_FPS();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_DeltaTime();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_TimeSinceLaunch();
		#endregion
	}
}