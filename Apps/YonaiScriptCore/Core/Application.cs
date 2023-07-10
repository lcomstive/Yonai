using System.Runtime.CompilerServices;

namespace Yonai
{
	public enum Platform : byte
	{
		Unknown,
		Windows,
		Mac,
		Linux,
		iOS,
		Android,
		Unix // Other Unix platform
	}

	public enum BuildType : byte { Debug, Release }

	public static class Application
	{
		public static Platform Platform => (Platform)_GetPlatform();
		public static BuildType Configuration => (BuildType)_GetBuildType();

		public static bool IsDesktop => _IsDesktop();
		public static bool IsMobile => !_IsDesktop();

		public static string ExecutablePath => _GetExecutablePath();
		public static string ExecutableDirectory => _GetExecutableDirectory();

		public static void Exit() => _Exit();

		/// <summary>
		/// Checks if an argument was passed to the application upon startup
		/// </summary>
		public static bool HasArg(string name) => _HasArg(name.ToLower());

		/// <summary>
		/// Gets an argument passed to the application during startup
		/// </summary>
		/// <param name="name">Name of the argument to check for</param>
		/// <param name="defaultValue">Value to return if argument is not found</param>
		public static string GetArg(string name, string defaultValue = "") => _GetArg(name.ToLower(), defaultValue ?? string.Empty);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Exit();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetExecutablePath();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetExecutableDirectory();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _HasArg(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern byte _GetBuildType();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern byte _GetPlatform();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsDesktop();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetArg(string name, string defaultValue);
		#endregion
	}
}
