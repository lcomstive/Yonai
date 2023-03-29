using System.Runtime.CompilerServices;

namespace AquaEditor
{
	public static class Scripting
	{
		public static bool DebuggingEnabled => _DebuggingEnabled();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Reload();
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DebuggingEnabled();

	}
}