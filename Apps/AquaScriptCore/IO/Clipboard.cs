using System.IO;
using System.Runtime.CompilerServices;

namespace AquaEngine.IO
{
	public static class Clipboard
	{
		public static void SetText(string contents) => _SetText(contents);
		public static string GetText() => _GetText();

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetText(string contents);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetText();
	}
}