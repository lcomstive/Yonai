namespace Yonai.Systems
{
	public class AudioSystem : NativeSystem
	{
		protected override void Start()		=> SceneManager.WorldChanged += OnWorldChanged;
		protected override void Destroyed() => SceneManager.WorldChanged -= OnWorldChanged;

		private void OnWorldChanged(World world, bool added)
		{
			if (!added || !IsEnabled)
				return;

			SoundSource[] sources = world.GetComponents<SoundSource>();
			foreach (SoundSource source in sources)
				if (source.PlayOnStart)
					source.State = SoundState.Playing;
		}
	}
}