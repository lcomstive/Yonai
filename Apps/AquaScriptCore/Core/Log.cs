using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public enum LogLevel : int
	{
		Trace = 0,
		Debug,
		Info,
		Warning,
		Error,
		Critical
	}

	public static class Log
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _NativeLog(string msg, int level);

		public static void Trace(string msg) => ProcessMessage(msg, LogLevel.Trace);
		public static void Debug(string msg) => ProcessMessage(msg, LogLevel.Debug);
		public static void Info(string msg) => ProcessMessage(msg, LogLevel.Info);
		public static void Warning(string msg) => ProcessMessage(msg, LogLevel.Warning);
		public static void Error(string msg) => ProcessMessage(msg, LogLevel.Error);
		public static void Critical(string msg) => ProcessMessage(msg, LogLevel.Critical);

		private static string FormatException(Exception e)
		{
			string msg = string.Empty;
			if(e.InnerException != null)
				msg = FormatException(e.InnerException);
			msg += $"[{e.GetType().Name}] " + e.Message + "\n\n" + e.Source + "\n\t" + e.StackTrace;
			return msg;
		}

		public static void Exception(Exception e, string additionalInfo = "")
		{
			string msg = string.Empty;
			if (!string.IsNullOrEmpty(additionalInfo))
				msg = additionalInfo + "\n";
			msg += FormatException(e);
			ProcessMessage(msg, LogLevel.Critical);
		}

		private static void ProcessMessage(string msg, LogLevel level)
		{
			_NativeLog(msg, (int)level);
			Message?.Invoke(msg, level);
		}

		public delegate void OnMessage(string msg, LogLevel level);
		public static event OnMessage Message;
	}
}
