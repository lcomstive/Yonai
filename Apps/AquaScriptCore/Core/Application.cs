using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Application
	{
		public static void Exit() => _aqua_internal_Exit();

		/// <summary>
		/// Checks if an argument was passed to the application upon startup
		/// </summary>
		public static bool HasArg(string name) => _aqua_internal_HasArg(name.ToLower());

		/// <summary>
		/// Gets an argument passed to the application during startup
		/// </summary>
		/// <param name="name">Name of the argument to check for</param>
		/// <param name="defaultValue">Value to return if argument is not found</param>
		public static string GetArg(string name, string defaultValue = "") => _aqua_internal_GetArg(name.ToLower(), defaultValue);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_Exit();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_HasArg(string name);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _aqua_internal_GetArg(string name, string defaultValue);
		#endregion
	}
}
