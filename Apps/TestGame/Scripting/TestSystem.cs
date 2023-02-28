using System;
using AquaEngine;
using System.Diagnostics;
using AquaEngine.Graphics;

namespace TestGame
{
	struct ModelData
	{
		public Model Model;
		public float Scale;
	}
	
	public class TestSystem : AquaSystem
	{
		private Colour[] ColourValues =
		{
			Colour.White,
			Colour.Red,
			Colour.Green,
			Colour.Blue
		};

		private ModelData m_Model;
		private Mesh m_QuadMesh;
		private Mesh m_DupeMesh;
		private Material m_Material;
		private Shader m_SpriteShader, m_ModelShader;
		private Texture m_Texture1, m_Texture2;

		private VideoMode m_VideoMode;
		private VideoMode[] m_VideoModes;
		private int m_VideoModeIndex = 0;

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
			Log.Debug($"Loaded shader 'Shaders/NewSpriteShader' [{m_SpriteShader.ResourceID}]");

			m_ModelShader = Resource.Load<Shader>("Shaders/ModelShader", new ShaderStages()
			{
				VertexPath = "assets://Shaders/Unlit.vert",
				FragmentPath = "assets://Shaders/Unlit.frag"
			});
			Log.Debug($"Loaded shader 'Shaders/ModelShader' [{m_ModelShader.ResourceID}]");

			m_QuadMesh = Resource.Load<Mesh>("Meshes/Primitive/Quad");
			m_DupeMesh = Resource.Duplicate<Mesh>(m_QuadMesh, "Meshes/DupeQuad");

			// Load 3D models
			string modelPath = "assets://Models/Backpack/Survival_BackPack_2.fbx";
			m_Model = new ModelData()
			{
				Model = Resource.Load<Model>("Models/Backpack", modelPath),
				Scale = 1.0f
			};
			Log.Debug($"Loaded model \"{modelPath}\"");
			
			SpriteRenderer[] renderers = World.GetComponents<SpriteRenderer>();
			for (int i = 0; i < renderers.Length; i++)
			{
				renderers[i].Shader = m_SpriteShader;

				// Only do half of the renderers
				if (i >= renderers.Length / 2)
					break;
			}

			m_VideoMode = Screen.VideoMode;
			m_VideoModes = Screen.VideoModes;
			m_VideoModeIndex = m_VideoModes.Length - 1;

			Vector3 v = new Vector3(1, 2, 5);
			Log.Debug(v);
		}

		protected override void Update()
		{
			// Get all renderers in scene
			SpriteRenderer[] renderers = World.GetComponents<SpriteRenderer>();
			 
			if (Input.IsKeyPressed(Key.T))
				foreach (SpriteRenderer renderer in renderers)
					renderer.Sprite = (renderer.Sprite == m_Texture1 ? m_Texture2 : m_Texture1);

			m_SpriteShader.Set("multiplier", Time.TimeSinceLaunch);

			if (Input.IsKeyPressed(Key.F11))
				ToggleFullscreen();
			if (Input.IsKeyPressed(Key.F8))
				PreviousVideoMode();
			if (Input.IsKeyPressed(Key.F9))
				NextVideoMode();

			if (Input.IsKeyDown(Key.LeftControl) && Input.IsKeyDown(Key.Q))
				Application.Exit();

			if (Input.IsKeyDown(Key.Space))
				// CreateProjectileShot();
				CreateQuad();
			if(Input.IsKeyPressed(Key.N))
				CreateModel(m_Model);

			if (Input.IsKeyDown(Key.O))
				ProjectileSpread -= Time.DeltaTime;
			if (Input.IsKeyDown(Key.P))
				ProjectileSpread += Time.DeltaTime;

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

			Log.Debug($"Camera rotation: {cameraTransform.Rotation} | Transform rotation: {transform.Rotation}");

			SpriteRenderer renderer = e.AddComponent<SpriteRenderer>();
			renderer.Shader = m_SpriteShader;
			renderer.Sprite = m_Texture2;

			LerpTest lerpTest = e.AddComponent<LerpTest>();
			lerpTest.A = transform.Position - transform.Right * 2;
			lerpTest.B = transform.Position + transform.Right * 2;
			lerpTest.Speed = 3;
		}

		private void CreateModel(ModelData model)
		{
			Transform cameraTransform = Camera.Main.GetComponent<Transform>();

			foreach(Model.MeshData meshData in model.Model.Meshes)
			{
				Entity e = World.CreateEntity();
				Transform transform = e.AddComponent<Transform>();
				transform.Position = cameraTransform.Position + cameraTransform.Forward * 2.5f;
				transform.Scale = new Vector3(model.Scale);
				transform.Rotation = Quaternion.Identity * Vector3.Down;

				MeshRenderer renderer = e.AddComponent<MeshRenderer>();
				renderer.Mesh = meshData.Mesh;
				
				meshData.Material.Shader = m_ModelShader;
				renderer.Material = meshData.Material;
			}
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
			renderer.Sprite = m_Texture2;

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

		private void NextVideoMode()
		{
			if (m_VideoModeIndex < m_VideoModes.Length - 1)
				Screen.VideoMode = (m_VideoMode = m_VideoModes[++m_VideoModeIndex]);
		}

		private void PreviousVideoMode()
		{
			if (m_VideoModeIndex > 0)
				Screen.VideoMode = (m_VideoMode = m_VideoModes[--m_VideoModeIndex]);
		}
	}
}
