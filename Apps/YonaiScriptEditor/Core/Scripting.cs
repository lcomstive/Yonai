using System.Reflection;
using System.Runtime.CompilerServices;

namespace YonaiEditor
{
	public static class Scripting
	{
		public static bool DebuggingEnabled => _DebuggingEnabled();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern Assembly LoadAssembly(string path, bool shouldWatch = false);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool IsAssemblyLoaded(string path);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Reload();
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DebuggingEnabled();

	}
}