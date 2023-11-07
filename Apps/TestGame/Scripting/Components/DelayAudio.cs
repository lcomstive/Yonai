using Yonai;

namespace TestGame.Components
{
	public class DelayAudio : Component
	{
		/// <summary>
		/// Delay before playing audio
		/// </summary>
		public float Seconds = 1.0f;

		/// <summary>
		/// When true, starts attached <see cref="AudioSource"/>
		/// every time <see cref="Seconds"/> has elapsed
		/// </summary>
		public bool Repeat = false;

		/// <summary>
		/// Time accumulator
		/// </summary>
		internal float TimePassed = 0.0f;
	}
}
