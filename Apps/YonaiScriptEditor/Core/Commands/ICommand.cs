using System;

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
}