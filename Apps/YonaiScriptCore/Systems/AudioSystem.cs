namespace Yonai.Systems
{
	public class AudioSystem : NativeSystem
	{
		public override bool IsGlobal => true;

		protected override void Start()		=> SceneManager.WorldChanged += OnWorldChanged;
		protected override void Destroyed() => SceneManager.WorldChanged -= OnWorldChanged;

		private void OnWorldChanged(World world, bool added)
		{
			if (!added || !IsEnabled)
				return;

			AudioSource[] sources = world.GetComponents<AudioSource>();
			foreach (AudioSource source in sources)
				if (source.PlayOnStart)
					source.State = SoundState.Playing;
		}
	}
}