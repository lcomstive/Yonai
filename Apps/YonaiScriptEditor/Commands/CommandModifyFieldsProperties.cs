using System.Reflection;

namespace YonaiEditor.Commands
{
	/// <summary>
	/// Stores <see cref="FieldInfo"/> to directly modify its value.
	/// </summary>
	public struct CommandModifyField : ICommand
	{
		private object Instance;
		private FieldInfo Field;
		private object PreviousValue;
		private object NewValue;

		public CommandModifyField(object instance, FieldInfo field, object previousValue, object newValue)
		{
			Field = field;
			Instance = instance;
			NewValue = newValue;
			PreviousValue = previousValue;
		}

		public void Execute() => Field.SetValue(Instance, NewValue);

		public void Undo() => Field.SetValue(Instance, PreviousValue);
	}

	/// <summary>
	/// Stores <see cref="PropertyInfo"/> to directly modify its value.
	/// </summary>
	public struct CommandModifyProperty : ICommand
	{
		private object Instance;
		private PropertyInfo Field;
		private object PreviousValue;
		private object NewValue;

		public CommandModifyProperty(object instance, PropertyInfo property, object previousValue, object newValue)
		{
			Field = property;
			Instance = instance;
			NewValue = newValue;
			PreviousValue = previousValue;
		}

		public void Execute() => Field.SetValue(Instance, NewValue);

		public void Undo() => Field.SetValue(Instance, PreviousValue);
	}
}