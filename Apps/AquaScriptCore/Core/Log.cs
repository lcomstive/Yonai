using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Log
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _aqua_internal_NativeLog(string msg, int level);

		public static void Debug(string msg) => _aqua_internal_NativeLog(msg, 0);
		public static void Info(string msg) => _aqua_internal_NativeLog(msg, 1);
		public static void Warning(string msg) => _aqua_internal_NativeLog(msg, 2);
		public static void Error(string msg) => _aqua_internal_NativeLog(msg, 3);
		public static void Critical(string msg) => _aqua_internal_NativeLog(msg, 4);

		public static void Exception(Exception e)
		{
			string msg = e.Message + "\n\n";
			msg += e.Source + "\n\t";
			msg += e.StackTrace;
			_aqua_internal_NativeLog(msg, 4);
		}
	}
}
