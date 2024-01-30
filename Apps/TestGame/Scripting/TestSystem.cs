using TestGame.Components;
using TestGame.Systems;
using Yonai;
using Yonai.Graphics;

namespace TestGame
{
	public class TestSystem : YonaiSystem
	{
		private Shader m_SpriteShader;
		private Texture m_Texture;

		private AudioData m_Sound;
		private AudioMixer m_SoundMixer;
		private uint m_AudioDeviceIndex = 0;
		private float m_Pitch = 1.0f;

		protected override void Enabled()
		{
			Log.Debug("TestSystem.Enabled() ---------");
			Camera[] cameras = World.GetComponents<Camera>();
			if (cameras.Length > 0)
			{
				Camera.Main = cameras[0];
				Log.Debug("Main camera set to " + Camera.Main.GetComponent<NameComponent>().Name);
			}

			// Add<SteamSystem>().Enable();

			/*
			World.AddSystem<CameraControlSystem>();
			World.AddSystem<DelayAudioSystem>();
			World.AddSystem<MoveAlongPathSystem>();
			*/

			// Load test texture
			m_Texture = Resource.Load<Texture>("assets://Textures/texture_09.png");

			// Load test sprite shader
			ShaderImportSettings shaderStages = new ShaderImportSettings();
			shaderStages.VertexPath = "assets://Shaders/Sprite.vert";
			shaderStages.FragmentPath = "assets://Shaders/NewSprite.frag";
			m_SpriteShader = Resource.Load<Shader>("assets://Shaders/NewSpriteShader", shaderStages);

			Vector3 v = new Vector3(1, 2, 5);
			Log.Debug(v);

			m_AudioDeviceIndex = Audio.OutputDevice.Index;
			m_Sound = Resource.Load<AudioData>("assets://Audio/Fall.mp3");

			m_SoundMixer = Resource.Load<AudioMixer>("assets://Audio/Mixers/SFX.mixer");
		}

		protected override void Destroyed() => Remove<SteamSystem>();

		protected override void Update()
		{			
			m_SpriteShader.Set("multiplier", Time.TimeSinceLaunch);

			if (Input.IsKeyDown(Key.LeftControl) && Input.IsKeyDown(Key.Q))
				Application.Exit();

			if (Input.IsKeyDown(Key.Space))
				// CreateProjectileShot();
				CreateQuad();

			if (Input.IsKeyDown(Key.O))
				ProjectileSpread -= Time.DeltaTime;
			if (Input.IsKeyDown(Key.P))
				ProjectileSpread += Time.DeltaTime;

			if (Input.IsKeyPressed(Key.Num9))
				SetNextAudioDevice();
			if (Input.IsKeyPressed(Key.Num8))
				SetPreviousAudioDevice();

			if (Input.IsMousePressed(MouseButton.Left))
				CreateProjectile();

			m_SoundMixer.Volume += Input.ScrollDelta / 100.0f;
		}

		private float ProjectileForce = 20.0f;
		private float ProjectileSpread = 1.0f;
		private static readonly Vector2 ProjectileShotSize = new Vector2(5, 5);

		private void CreateQuad()
		{
			Transform cameraTransform = Camera.Main.GetComponent<Transform>();
			Entity e = World.CreateEntity();
			Transform transform = e.AddComponent<Transform>();
			
			transform.Position = cameraTransform.Position + cameraTransform.Forward * 2.5f;
			// transform.Scale = new Vector3(0.25f);
			transform.LocalRotation = Quaternion.FromEuler(-cameraTransform.Rotation.Euler);

			SpriteRenderer renderer = e.AddComponent<SpriteRenderer>();
			renderer.Shader = m_SpriteShader;
			renderer.Sprite = m_Texture;

			LerpTest lerpTest = e.AddComponent<LerpTest>();
			lerpTest.A = transform.Position - transform.Right * 2;
			lerpTest.B = transform.Position + transform.Right * 2;
			lerpTest.Speed = 3;
		}

 		private void CreateProjectileShot()
		{
			for (float x = 0; x < ProjectileShotSize.x; x++)
			{
				for (float y = 0; y < ProjectileShotSize.y; y++)
				{
					Entity e = CreateProjectile();
					Vector3 force = new Vector3(
						x - ProjectileShotSize.x / 2.0f,
						y - ProjectileShotSize.y / 2.0f,
						1.0f
						);

					force *= ProjectileSpread;
					force *= ProjectileForce / 4.0f;
					e.GetComponent<RigidbodyTest>().AddForce(force);
					e.GetComponent<RigidbodyTest>().Gravity = 0.0f;
				}
			}
		}

		private Entity CreateProjectile()
		{
			Transform cameraTransform = Camera.Main.GetComponent<Transform>();
			Entity e = World.CreateEntity();
			Transform transform = e.AddComponent<Transform>();
			transform.Position = cameraTransform.Position + cameraTransform.Forward;
			transform.LocalScale = new Vector3(0.1f);
			transform.LocalRotation = cameraTransform.Rotation;

			SpriteRenderer renderer = e.AddComponent<SpriteRenderer>();
			renderer.Shader = m_SpriteShader;
			renderer.Sprite = m_Texture;

			e.AddComponent<RigidbodyTest>().AddForce(cameraTransform.Forward * ProjectileForce);
			e.AddComponent<DestroyAfterSeconds>().StartCountdown(5.0f);

			AudioSource source = e.AddComponent<AudioSource>();
			source.Sound = m_Sound;
			source.Is3D = false;
			source.Pitch += Random.Range(1.0f);
			source.Mixer = m_SoundMixer;
			source.Volume = Random.Range(0.5f, 1.0f);
			source.Play();

			return e;
		}

		private IVector2 m_WindowedResolution = IVector2.Zero;
		private void ToggleFullscreen()
		{
			VideoMode[] modes = Screen.VideoModes;

			// If windowed, set resolution to match monitor prior to going fullscreen
			if (Window.Fullscreen == FullscreenMode.Windowed)
			{
				m_WindowedResolution = Window.Resolution;
				Window.Resolution = modes[modes.Length - 1].Resolution;
			}

			Window.CycleFullscreen();

			if (Window.Fullscreen == FullscreenMode.Windowed)
				Window.Resolution = m_WindowedResolution;
			Window.CenterOnDisplay();
		}

		private void SetNextAudioDevice()
		{
			m_AudioDeviceIndex++;
			if (m_AudioDeviceIndex >= Audio.Devices.Length)
				m_AudioDeviceIndex = 0;
			Audio.OutputDevice = Audio.Devices[m_AudioDeviceIndex];
		}

		private void SetPreviousAudioDevice()
		{
			if (m_AudioDeviceIndex > 0)
				m_AudioDeviceIndex--;
			else
				m_AudioDeviceIndex = (uint)Audio.Devices.Length - 1;
			Audio.OutputDevice = Audio.Devices[m_AudioDeviceIndex];
		}
	}
}
