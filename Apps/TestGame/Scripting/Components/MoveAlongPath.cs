using Yonai;

namespace TestGame.Components
{
	public class MoveAlongPath : Component
	{
		public float Speed = 1.0f;
		public Vector3 PointA = new Vector3(-10, 0, 0);
		public Vector3 PointB = new Vector3( 10, 0, 0);

		/// <summary>
		/// Toggles when reaching end of path,
		/// signals to turn around
		/// </summary>
		internal bool m_Backwards = false;

		/// <summary>
		/// Current position, range between 0 and 1
		/// </summary>
		internal float m_Position = 0.0f;
	}
}
