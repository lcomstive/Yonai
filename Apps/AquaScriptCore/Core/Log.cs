using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Log
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _NativeLog(string msg, int level);

		public static void Debug(string msg) => _NativeLog(msg, 0);
		public static void Info(string msg) => _NativeLog(msg, 1);
		public static void Warning(string msg) => _NativeLog(msg, 2);
		public static void Error(string msg) => _NativeLog(msg, 3);
		public static void Critical(string msg) => _NativeLog(msg, 4);

		public static void Exception(Exception e)
		{
			string msg = e.Message + "\n\n";
			msg += e.Source + "\n\t";
			msg += e.StackTrace;
			_NativeLog(msg, 4);
		}
	}
}
