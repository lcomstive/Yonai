using System;
using System.Runtime.CompilerServices;

namespace Yonai
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
		/// <summary>
		/// When enabled, prints stack traces for all logged messages to the native log
		/// </summary>
		public static bool PrintStackTraces { get; set; } = false;

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _NativeLog(string msg, int level);

		public static void Trace(string msg) => ProcessMessage(msg, LogLevel.Trace);
		public static void Debug(string msg) => ProcessMessage(msg, LogLevel.Debug);
		public static void Info(string msg) => ProcessMessage(msg, LogLevel.Info);
		public static void Warning(string msg) => ProcessMessage(msg, LogLevel.Warning);
		public static void Error(string msg) => ProcessMessage(msg, LogLevel.Error);
		public static void Critical(string msg) => ProcessMessage(msg, LogLevel.Critical);

		/// <summary>
		/// Prints the current stacktrace to the trace debug output
		/// </summary>
		/// <param name="additionalMsg">Message to output prior to the stacktrace</param>
		public static void OutputStack(string additionalMsg = "")
		{
			bool stackTrace = PrintStackTraces;
			PrintStackTraces = true;
			ProcessMessage(additionalMsg, LogLevel.Trace);
			PrintStackTraces = stackTrace;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Flush();

		public static void Flush() => _Flush();

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
			string nativeMsg = msg;
			if (PrintStackTraces)
				nativeMsg += $"\n\t{new System.Diagnostics.StackTrace(2, true)}\n";
			_NativeLog(nativeMsg, (int)level);

			Message?.Invoke(msg, level);
		}

		public delegate void OnMessage(string msg, LogLevel level);
		public static event OnMessage Message;
	}
}
