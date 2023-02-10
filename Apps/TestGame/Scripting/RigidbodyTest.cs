using AquaEngine;

namespace TestGame
{
	public class RigidbodyTest : Component
	{
		public float Gravity = 9.81f;

		private Transform m_Transform = null;
		private Vector3 m_Force = Vector3.Zero;
		private float m_Rotation = 0;

		protected override void Start() => m_Transform = GetComponent<Transform>();

		protected override void Update()
		{
			m_Force += Vector3.Down * Gravity * Time.DeltaTime;
			m_Transform.Position += m_Force * Time.DeltaTime * Time.DeltaTime;

			m_Rotation += Time.DeltaTime;
			m_Transform.Rotation = Quaternion.FromEuler(Vector3.Up * m_Rotation);
		}

		public void AddForce(Vector3 force) => m_Force += force;
	}
}
