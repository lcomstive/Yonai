using System;
using System.Collections.Generic;
using Yonai;

namespace YonaiEditor.Commands
{
	public interface ICommand
	{
		void Execute();
		void Undo();
	}

	public struct CommandRelay : ICommand
	{
		private Action m_Execute;
		private Action m_Undo;

		public CommandRelay(Action execute, Action undo)
		{
			m_Undo = undo;
			m_Execute = execute;
		}

		public void Execute() => m_Execute?.Invoke();
		public void Undo() => m_Undo?.Invoke();
	}

	/// <summary>
	/// Allows undoing and redoing of <see cref="ICommand"/>s
	/// </summary>
	public static class CommandHistory
	{
		private const uint MaxHistoryLength = 128;

		/// <summary>
		/// Fixed-length array of commands, used cyclically for undoing and redoing
		/// </summary>
		private static List<ICommand> s_CommandBuffer = new List<ICommand>();
		private static int s_CommandIndex = 0;

		public static void Execute(ICommand command)
		{
			// If redo history is available, clear it
			if (s_CommandIndex < s_CommandBuffer.Count)
				s_CommandBuffer.RemoveRange(s_CommandIndex, s_CommandBuffer.Count - s_CommandIndex);

			s_CommandBuffer.Add(command);
			command.Execute();
			s_CommandIndex++;

			// Remove first element if too many items
			if (s_CommandBuffer.Count > MaxHistoryLength)
			{
				s_CommandBuffer.RemoveAt(0);
				s_CommandIndex--;
			}
		}

		[MenuItem("File/Undo", Shortcut = "CTRL + Z")]
		public static void Undo()
		{
			if (s_CommandBuffer.Count == 0 ||
				s_CommandIndex == 0)
				return; // Nothing to undo

			s_CommandBuffer[--s_CommandIndex].Undo();
		}

		[MenuItem("File/Redo", Shortcut = "CTRL + Y")]
		public static void Redo()
		{
			if (s_CommandBuffer.Count == 0 ||
				s_CommandIndex >= s_CommandBuffer.Count)
				return; // Nothing left in buffer to redo

			s_CommandBuffer[s_CommandIndex++].Execute();
		}

		public static void Clear()
		{
			s_CommandIndex = 0;
			s_CommandBuffer.Clear();
		}
	}
}