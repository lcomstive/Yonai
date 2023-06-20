using System;
using AquaEngine;

namespace TestGame
{
	public class LerpTest : Behaviour
	{
		public Vector3 A = Vector3.Zero, B = Vector3.Zero;
		public float Speed = 1.0f;

		private Transform m_Transform = null;
		private Vector3 m_StartPosition = Vector3.Zero;

		public override void OnEnable() => m_Transform = GetComponent<Transform>();
		public override void Start() => m_StartPosition = m_Transform.LocalPosition;

		public float m_TotalTime = 0;
		public float m_DeltaTime = 0;

		public override void Update()
		{
			// Check if there is a distance between targets
			if (Vector3.Distance(A, B) <= 0.1f || !m_Transform)
				return;

			// Get time for lerping
			m_TotalTime += Time.DeltaTime;
			float t = /* Time.TimeSinceLaunch */ m_TotalTime * Speed;
			m_DeltaTime = Time.DeltaTime;

			// Convert time range from [-1, 1] to [0, 1]
			t = (float)Math.Cos(t) / 2.0f + 0.5f;
			
			// Move to point between A and B
			m_Transform.LocalPosition = m_StartPosition + Vector3.Lerp(A, B, t);
		}
	}
}
