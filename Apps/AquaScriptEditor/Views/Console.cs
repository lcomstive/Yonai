using System;
using AquaEngine;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AquaEditor.Views
{
	public class ConsoleView : View
	{
		private class ConsoleMessage
		{
			public string Message;
			public LogLevel Level;
			public StackTrace Trace;
			public DateTime TimeStamp;
			public bool ShowTrace;

			public ConsoleMessage(string message, LogLevel level)
			{
				Level = level;
				Message = message;
				ShowTrace = false;
				if(RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
					TimeStamp = DateTime.Now;
				else
					TimeStamp = new DateTime(0);
				Trace = new StackTrace(4, true);
			}
		}

		private const float IndicatorPadding = 2;
		private const float IndicatorThickness = 2.5f;
		private const float StacktraceDropdownSize = 17.5f;

		private static List<ConsoleMessage> m_Messages = new List<ConsoleMessage>();

		[MenuItem("Window/Console")]
		private static void MenuCallback() => EditorUIService.Open<ConsoleView>();

		protected override void Opened()
		{
			Log.Message += OnLogMessage;

			Log.Trace("This is\na test\nMulti\n\tline\n\nmessage");
			// Log.Warning("And this message is a very, very, very long message that should fill up the entire length of the window because it just keeps on going and going and going...");
		}

		protected override void Closed() => Log.Message -= OnLogMessage;

		private static void OnLogMessage(string msg, LogLevel level) => m_Messages.Add(new ConsoleMessage(msg, level));

		protected override void Draw()
		{
			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowPadding, Vector2.Zero);
			ImGUI.PushStyleColour(ImGUI.StyleColour.FrameBg, Colour.None);
			ImGUI.DrawListPushClipRectFullScreen();

			bool isOpen = true;
			if (ImGUI.Begin("Console", ref isOpen))
			{
				DrawTopBar();
				ImGUI.PushStyleVar(ImGUI.StyleVar.FramePadding, Vector2.Zero);
				
				ImGUI.BeginChild("ConsoleMessages", false, ImGUI.WindowFlags.HorizontalScrollbar);
				for (int i = 0; i < m_Messages.Count; i++)
				{
					ConsoleMessage message = m_Messages[i];
					string msg = $"{message.Message}\0";
					string timestamp = message.TimeStamp.ToString("hh:mm tt");
					Vector2 size = ImGUI.CalculateTextWidth(msg);
					Vector2 timestampSize = ImGUI.CalculateTextWidth(timestamp);
					size.x = Math.Max(size.x, ImGUI.ContentRegionAvailable.x - IndicatorThickness - IndicatorPadding - StacktraceDropdownSize - timestampSize.x - 15);

					DrawSeverityIndicator(message.Level, size.y);
					ImGUI.HorizontalSpace(StacktraceDropdownSize + timestampSize.x);
					ImGUI.InputTextMultiline($"##ConsoleMessage[{i}]", ref msg, size, msg.Length, ImGUI.TextFlags.ReadOnly);

					// Show stacktrace foldout
					ImGUI.SameLine();
					ImGUI.SetCursorPos(IndicatorPadding + IndicatorThickness + 5, ImGUI.GetCursorPosY() + size.y - StacktraceDropdownSize);
					ImGUI.PushID($"ConsoleStackDropdown[{i}]");
					if (ImGUI.ButtonImage(
						message.ShowTrace ? Icons.Up : Icons.Down,
						new Vector2(StacktraceDropdownSize, StacktraceDropdownSize)))
						m_Messages[i].ShowTrace = !message.ShowTrace;
					ImGUI.PopID();

					ImGUI.SameLine();
					ImGUI.SetCursorPos(IndicatorPadding + IndicatorThickness + 10 + StacktraceDropdownSize, ImGUI.GetCursorPosY() + size.y - StacktraceDropdownSize);
					ImGUI.Text(timestamp, Colour.Grey);

					if (message.ShowTrace)
					{
						ImGUI.HorizontalSpace(5);
						ImGUI.Text(message.Trace.ToString());
					}

					ImGUI.Space();
				}
				ImGUI.EndChild();
				ImGUI.PopStyleVar();
			}

			ImGUI.End();
			ImGUI.PopStyleColour();
			ImGUI.PopStyleVar();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<ConsoleView>();
		}

		private static readonly Dictionary<LogLevel, Colour> SeverityColours = new Dictionary<LogLevel, Colour>()
		{
			{ LogLevel.Trace,	 Colour.Grey },
			{ LogLevel.Debug,	 Colour.Green },
			{ LogLevel.Info,	 Colour.White},
			{ LogLevel.Warning,  Colour.Yellow },
			{ LogLevel.Error,	 Colour.Red },
			{ LogLevel.Critical, Colour.Turqoise },
		};

		private void DrawSeverityIndicator(LogLevel level, float height)
		{
			Vector2 origin = ImGUI.GetCursorScreenPos();
			Vector2 size = new Vector2(IndicatorThickness, -height);
			origin += new Vector2(IndicatorPadding, height);
			Colour colour = SeverityColours.ContainsKey(level) ? SeverityColours[level] : Colour.Magenta;

			ImGUI.AddRectFilled(origin, origin + size, colour);

			ImGUI.Dummy(size);
			ImGUI.SameLine();			
		}

		private const float TopBarHeight = 35;
		private void DrawTopBar()
		{
			ImGUI.BeginChild("ConsoleTopBar", new Vector2(0, TopBarHeight), true);
			ImGUI.SetCursorPos(5, 5);

			if(ImGUI.Button("Clear")) m_Messages.Clear();

			ImGUI.EndChild();
		}
	}
}