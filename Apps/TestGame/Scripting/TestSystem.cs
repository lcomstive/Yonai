using System;
using System.Diagnostics;
using AquaEngine;
using AquaEngine.Graphics;

namespace TestGame
{
	public class TestSystem : AquaSystem
	{
		private Colour[] ColourValues =
		{
			Colour.White,
			Colour.Red,
			Colour.Green,
			Colour.Blue
		};

		private Shader m_SpriteShader;
		private Texture m_Texture1, m_Texture2;

		protected override void Enabled() => World.AddSystem<CameraControlSystem>();
		protected override void Disabled() => World.RemoveSystem<CameraControlSystem>();

		protected override void Start()
		{
			/*
			Log.Debug("Colours:");
			foreach(var colour in ColourValues)
					Log.Debug($"{colour.r}, {colour.g}, {colour.b}, {colour.a}");

			Colour c = new Colour(1, 0, 1, 1);
			c.g = 0.75f;
			Log.Debug($"{c} ({c.r}, {c.g})");

			TestComponent[] testComponents = World.GetComponents<TestComponent>();
			for (int i = 0; i < testComponents.Length; i++)
			{
				testComponents[i].Value = ColourValues[i % ColourValues.Length];
				Log.Debug($"TestComponent#{i}[{testComponents[i].Entity}]: {testComponents[i].Value}, {testComponents[i].ValueChangeSpeed}");
			}

			Log.Info($"Test components: {testComponents.Length}");
			*/

			// Load test texture
			string vfsPath = "assets://Textures/texture_09.png";
			Log.Info($"Checking for '{vfsPath}': " + (VFS.Exists(vfsPath) ? "Exists" : "Not found"));

			m_Texture1 = Resource.Load<Texture>("Texture/Test", vfsPath);
			m_Texture2 = Resource.Load<Texture>("Texture/Test2", vfsPath);

			Log.Debug($"Loaded textures [{(uint)m_Texture1}] and [{(uint)m_Texture2}]");

			// Load test sprite shader
			ShaderStages shaderStages = new ShaderStages()
			{
				VertexPath = "assets://Shaders/Sprite.vert",
				FragmentPath = "assets://Shaders/NewSprite.frag",
			};
			m_SpriteShader = Resource.Load<Shader>("Shaders/NewSpriteShader", shaderStages);

			SpriteRenderer[] renderers = World.GetComponents<SpriteRenderer>();
			for(int i = 0; i < renderers.Length; i++)
			{
				renderers[i].Shader = m_SpriteShader;

				// Only do half of the renderers
				if (i >= renderers.Length / 2)
					break;
			}
		}

		protected override void Update()
		{
			// Get all entities that have Transform & SpriteRenderer components
			(Transform[] transform,
			 SpriteRenderer[] renderers,
			 TestComponent[] testComponents)
			 = World.GetComponents<Transform, SpriteRenderer, TestComponent>();

			m_SpriteShader.Set("multiplier", Time.TimeSinceLaunch);

			if (Input.IsKeyPressed(Key.F11))
				Window.CycleFullscreen();

			if (Input.IsKeyDown(Key.LeftControl) && Input.IsKeyDown(Key.Q))
				Application.Exit();

			if (Input.IsKeyPressed(Key.T))
				foreach (SpriteRenderer renderer in renderers)
					renderer.Sprite = (renderer.Sprite == m_Texture1 ? m_Texture2 : m_Texture1);

			if (Input.IsKeyPressed(Key.Space))
				CreateProjectile();
		}

		private void CreateProjectile()
		{
			Transform cameraTransform = Camera.Main.GetComponent<Transform>();
			Entity e = World.CreateEntity();
			Transform transform = e.AddComponent<Transform>();
			transform.Position = cameraTransform.Position + cameraTransform.Forward;
			transform.Scale = new Vector3(0.1f);
			transform.Rotation = cameraTransform.Rotation;

			SpriteRenderer renderer = e.AddComponent<SpriteRenderer>();
			renderer.Shader = m_SpriteShader;
			renderer.Sprite = m_Texture2;

			e.AddComponent<RigidbodyTest>().AddForce(cameraTransform.Forward * 5.0f);

			e.AddComponent<DestroyAfterSeconds>().StartCountdown(5.0f);
		}
	}
}
