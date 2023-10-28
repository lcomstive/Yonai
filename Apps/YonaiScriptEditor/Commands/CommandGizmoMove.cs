using Yonai;

namespace YonaiEditor.Commands
{
	/// <summary>
	/// Stores local position, rotation and scale
	/// </summary>
	public struct CommandGizmoMove : ICommand
	{
		private Transform m_Target;
		private Vector3 m_OldPosition, m_Position;
		private Vector3 m_OldScale, m_Scale;
		private Quaternion m_OldRotation, m_Rotation;

		/// <summary>
		/// Stores <paramref name="target"/>'s current local position, rotation and scale for execution.
		/// When undoing, sets <paramref name="target"/> to <paramref name="oldPosition"/>, <paramref name="oldScale"/> and <paramref name="oldRotation"/>.
		/// </summary>
		/// <param name="target">Target <see cref="Transform"/> component to apply changes to</param>
		/// <param name="oldPosition">Previous local position</param>
		/// <param name="oldScale">Previous local scale</param>
		/// <param name="oldRotation">Previous local rotation</param>
		public CommandGizmoMove(Transform target, Vector3 oldPosition, Vector3 oldScale, Quaternion oldRotation)
		{
			m_Target = target;
			m_OldScale = oldScale;
			m_OldPosition = oldPosition;
			m_OldRotation = oldRotation;

			m_Scale = m_Target.LocalScale;
			m_Position = m_Target.LocalPosition;
			m_Rotation = m_Target.LocalRotation;
		}

		public void Execute()
		{
			m_Target.LocalScale		= m_Scale;
			m_Target.LocalPosition	= m_Position;
			m_Target.LocalRotation	= m_Rotation;
		}

		public void Undo()
		{
			m_Target.LocalScale		= m_OldScale;
			m_Target.LocalPosition	= m_OldPosition;
			m_Target.LocalRotation	= m_OldRotation;
		}
	}
}