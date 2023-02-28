using System;
using AquaEngine;

namespace TestGame
{
	public class LerpTest : Component
	{
		public Vector3 A = Vector3.Zero, B = Vector3.Zero;
		public float Speed = 1.0f;

		private Transform m_Transform = null;

		protected override void Enabled() => m_Transform = GetComponent<Transform>();
		
		protected override void Update()
		{
			// Check if there is a distance between targets
			if (Vector3.Distance(A, B) <= 0.1f || !m_Transform)
				return;
			
			// Get time for lerping
			float t = Time.TimeSinceLaunch * Speed;
			
			// Convert time range from [-1, 1] to [0, 1]
			t = (float)Math.Cos(t) / 2.0f + 0.5f;
			
			// Move to point between A and B
			m_Transform.Position = Vector3.Lerp(A, B, t);
		}
	}
}
