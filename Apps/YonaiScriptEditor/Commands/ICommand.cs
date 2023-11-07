namespace YonaiEditor.Commands
{
	/// <summary>
	/// Allows execution of code and the ability to undo it,
	/// via <see cref="CommandHistory"/>.
	/// </summary>
	public interface ICommand
	{
		void Execute();
		void Undo();
	}
}