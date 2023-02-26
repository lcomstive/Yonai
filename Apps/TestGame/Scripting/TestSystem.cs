﻿using System;
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

		private Mesh m_QuadMesh;
		private Shader m_SpriteShader;
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

			m_QuadMesh = Resource.Load<Mesh>("Meshes/Primitive/Quad");

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
		}

		protected override void Update()
		{
			/*
			// Get all entities that have Transform & SpriteRenderer components
			(Transform[] transform,
			 SpriteRenderer[] renderers,
			 TestComponent[] testComponents)
			 = World.GetComponents<Transform, SpriteRenderer, TestComponent>();
			 
			if (Input.IsKeyPressed(Key.T))
				foreach (SpriteRenderer renderer in renderers)
					renderer.Sprite = (renderer.Sprite == m_Texture1 ? m_Texture2 : m_Texture1);

			 */

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

			if (Input.IsKeyDown(Key.O))
				ProjectileSpread -= Time.DeltaTime;
			if (Input.IsKeyDown(Key.P))
				ProjectileSpread += Time.DeltaTime;

			if (Input.IsKeyDown(Key.M))
			{
				Mesh.Vertex[] vertices = m_QuadMesh.Vertices;
				vertices[0].Position += Vector3.Up * Time.DeltaTime * 10.0f;
				m_QuadMesh.UpdateVertices();
			}
			if (Input.IsKeyDown(Key.N))
			{
				Mesh.Vertex[] vertices = m_QuadMesh.Vertices;
				vertices[0].Position += Vector3.Down * Time.DeltaTime * 20.0f;
				m_QuadMesh.UpdateVertices();
			}
		}

		private float ProjectileForce = 20.0f;
		private float ProjectileSpread = 1.0f;
		private static readonly Vector2 ProjectileShotSize = new Vector2(5, 5);

		private void CreateQuad()
		{
			Transform cameraTransform = Camera.Main.GetComponent<Transform>();

			Entity e = World.CreateEntity();
			Transform transform = e.AddComponent<Transform>();
			transform.Position = cameraTransform.Position + cameraTransform.Forward * 0.75f;
			transform.Scale = new Vector3(0.25f);

			SpriteRenderer renderer = e.AddComponent<SpriteRenderer>();
			renderer.Shader = m_SpriteShader;
			renderer.Sprite = m_Texture2;
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
