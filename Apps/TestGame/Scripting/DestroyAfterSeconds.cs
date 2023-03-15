using AquaEngine;

namespace TestGame
{
	public class DestroyAfterSeconds : Component
	{
		private float m_SecondsRemaining = -1;
		private bool m_Ticking = false;

		public void StartCountdown(float seconds)
		{
			m_SecondsRemaining = seconds;
			m_Ticking = true;
		}

		public void PauseCountdown() => m_Ticking = false;

		public void StopCountdown()
		{
			m_Ticking = false;
			m_SecondsRemaining = -1;
		}

		public void ResumeCountdown()
		{
			if (m_SecondsRemaining > 0)
				m_Ticking = true;
		}

		protected override void Update()
		{
			if (!m_Ticking)
				return;

			m_SecondsRemaining -= Time.DeltaTime;
			
			if (m_SecondsRemaining <= 0)
				Entity.Destroy();
		}
	}
}
