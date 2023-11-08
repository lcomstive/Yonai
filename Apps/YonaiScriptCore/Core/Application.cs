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

	public struct Version
	{
		public int Major;
		public int Minor;
		public int Patch;
		public string Branch;
		public string Revision;

		public Version(int major, int minor = 0, int patch = 0)
		{
			Major = major;
			Minor = minor;
			Patch = patch;
			Branch = Revision = string.Empty;
		}

		public override string ToString() => $"{Major}.{Minor}.{Patch}";
		public string ToLongString() => $"{Major}.{Minor}.{Patch}-{Revision} [{Branch}]";
	}

	public enum BuildType : byte { Debug, Release }

	public static class Application
	{
		public static Platform Platform => (Platform)_GetPlatform();
		public static BuildType Configuration => (BuildType)_GetBuildType();

		public static Version EngineVersion => new Version()
		{
			Major = _GetVersionMajor(),
			Minor = _GetVersionMinor(),
			Patch = _GetVersionPatch(),
			Revision = _GetVersionRevision(),
			Branch = _GetVersionBranch()
		};

		public static bool IsDesktop => _IsDesktop();
		public static bool IsMobile => !_IsDesktop();

		public static string ExecutablePath => _GetExecutablePath();
		public static string ExecutableDirectory => _GetExecutableDirectory();

		/// <summary>
		/// Directory with persistent storage.<br>
		/// Windows: %APPDATA%/Yonai/<br>
		/// Mac: ~/Library/Caches/Yonai/<br>
		/// Linux: ./.data/Yonai/
		/// </summary>
		public static string PersistentDirectory => _GetPersistentDirectory();

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
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPersistentDirectory();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _HasArg(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern byte _GetBuildType();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern byte _GetPlatform();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsDesktop();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetArg(string name, string defaultValue);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetVersionMajor();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetVersionMinor();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetVersionPatch();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetVersionRevision();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetVersionBranch();
		#endregion
	}
}
