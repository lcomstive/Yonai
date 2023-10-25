using Yonai;
using TestGame.Components;

namespace TestGame.Systems
{
	public class DelayAudioSystem : YonaiSystem
	{
		protected override void Update()
		{
			// Get all audio delays and sources in current scene
			(DelayAudio[] delays, AudioSource[] audioSources) =
				World.GetComponents<DelayAudio, AudioSource>();

			for(int i = 0; i < delays.Length; i++)
			{
				DelayAudio delay = delays[i];
				delay.TimePassed += Time.DeltaTime;

				if (delay.TimePassed < delay.Seconds) // Not enough time has passed
					continue;

				audioSources[i].State = SoundState.Playing;

				if (!delay.Repeat)
					// No need to repeat, this component is no longer needed
					delay.Entity.RemoveComponent<DelayAudio>();
				else
					// Reset time accumulator
					delay.TimePassed = 0.0f;
			}
		}
	}
}
