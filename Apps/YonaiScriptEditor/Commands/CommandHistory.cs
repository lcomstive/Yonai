using System.Collections.Generic;

namespace YonaiEditor.Commands
{
	/// <summary>
	/// Allows undoing and redoing of <see cref="ICommand"/>s
	/// </summary>
	public static class CommandHistory
	{
		/// <summary>
		/// Maximum amount of <see cref="ICommand"/>s to be stored in a single <see cref="CommandList"/>.
		/// </summary>
		private const int MaxHistoryLength = 128;

		/// <summary>
		/// First item is edit-time command list
		/// Second item is runtime command list
		/// </summary>
		private static CommandList[] s_CommandBuffer = new CommandList[2] { new CommandList(), new CommandList() };

		private static EditorState m_State => EditorService.State;

		private static CommandList GetCommandList(EditorState state) =>
			s_CommandBuffer[state == EditorState.Edit ? 0 : 1];

		public static void Execute(ICommand command) => GetCommandList(m_State).Execute(command);
		internal static void Execute(ICommand command, EditorState state) => GetCommandList(state).Execute(command);

		public static void Undo() => GetCommandList(m_State).Undo();
		public static void Redo() => GetCommandList(m_State).Redo();
		public static void Clear() => GetCommandList(m_State).Clear();

		private class CommandList
		{
			public int CommandIndex;
			public List<ICommand> Commands;

			public CommandList()
			{
				CommandIndex = 0;
				Commands = new List<ICommand>();
				Commands.Capacity = MaxHistoryLength; // Reserve enough memory for max items
			}

			public void Execute(ICommand command)
			{
				// If redo history is available, clear it
				if (CommandIndex < Commands.Count)
					Commands.RemoveRange(CommandIndex, Commands.Count - CommandIndex);

				Commands.Add(command);
				command.Execute();
				CommandIndex++;

				// Remove first element if too many items
				if (Commands.Count > MaxHistoryLength)
				{
					Commands.RemoveAt(0);
					CommandIndex--;
				}
			}

			public void Undo()
			{
				if (Commands.Count > 0 && CommandIndex > 0)
					Commands[--CommandIndex].Undo();
			}

			public void Redo()
			{
				if (Commands.Count > 0 && CommandIndex < Commands.Count)
					Commands[CommandIndex++].Execute();
			}

			public void Clear()
			{
				Commands.Clear();
				CommandIndex = 0;
			}
		}
	}
}