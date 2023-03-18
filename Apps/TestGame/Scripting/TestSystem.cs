using AquaEngine;
using AquaEngine.Graphics;

namespace TestGame
{
	public class TestSystem : AquaSystem
	{
		private Mesh m_QuadMesh;
		private Shader m_SpriteShader;
		private Texture m_Texture;

		private Sound m_Sound;
		private SoundSource m_SoundSource;
		private uint m_AudioDeviceIndex = 0;

		protected override void Enabled() => World.AddSystem<CameraControlSystem>();
		protected override void Disabled() => World.RemoveSystem<CameraControlSystem>();
		
		protected override void Start()
		{
			// Load test texture
			string vfsPath = "assets://Textures/texture_09.png";

			m_Texture = Resource.Load<Texture>("Texture/Test_Texture09", vfsPath);

			// Load test sprite shader
			ShaderStages shaderStages = new ShaderStages()
			{
				VertexPath = "assets://Shaders/Sprite.vert",
				FragmentPath = "assets://Shaders/NewSprite.frag",
			};
			m_SpriteShader = Resource.Load<Shader>("Shaders/NewSpriteShader", shaderStages);

			m_QuadMesh = Resource.Load<Mesh>("Meshes/Primitive/Quad");

			Vector3 v = new Vector3(1, 2, 5);
			Log.Debug(v);

			m_AudioDeviceIndex = Audio.OutputDevice.Index;

			m_Sound = Resource.Load<Sound>("Sounds/Bell", "assets://Audio/Bell.mp3");
			Entity soundEntity = World.CreateEntity();
			m_SoundSource = soundEntity.AddComponent<SoundSource>();
			m_SoundSource.Sound = m_Sound;
		}

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

			/*
			if (Input.IsKeyPressed(Key.Num5))
				m_SoundSource.Play();
			*/
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
			transform.Rotation = Quaternion.FromEuler(-cameraTransform.Rotation.Euler);

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
			transform.Scale = new Vector3(0.1f);
			transform.Rotation = cameraTransform.Rotation;

			SpriteRenderer renderer = e.AddComponent<SpriteRenderer>();
			renderer.Shader = m_SpriteShader;
			renderer.Sprite = m_Texture;

			e.AddComponent<RigidbodyTest>().AddForce(cameraTransform.Forward * ProjectileForce);
			e.AddComponent<DestroyAfterSeconds>().StartCountdown(10.0f);

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
