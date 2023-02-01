using System;
using AquaEngine;

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

			string vfsPath = "assets://Textures/texture_09.png";
			Log.Info($"Checking for '{vfsPath}': " + (VFS.Exists(vfsPath) ? "Exists" : "Not found"));

			AquaEngine.Graphics.Texture texture = Resource.Load<AquaEngine.Graphics.Texture>("Texture/Test", vfsPath);

			foreach (string resourcePath in new string[] {
				"Texture/Test",
				"Shaders/Sprite",
				"Should/Not/Exist",
			})
			{
				uint resourceID = Resource.GetID(resourcePath);
				Log.Info($"ID for resource at '{resourcePath}' is " + (resourceID == uint.MaxValue ? "invalid" : resourceID.ToString()));

				if(resourceID == uint.MaxValue)
				{
					Log.Warning($"Resource at path '{resourcePath}' was not found");
					continue;
				}

				if(Resource.GetPath(resourceID) != resourcePath)
				{
					Log.Warning("Getting path did not match original resource path?");
					continue;
				}

				Type resourceType = Resource.GetResourceType(resourceID);
				if(resourceType == null)
				{
					Log.Warning($"Could not get type for '{resourcePath}'");
					continue;
				}

				Log.Info($"Resource '{resourcePath}' [{resourceID}] " + resourceType.FullName);
			}
		}

		protected override void Update()
		{
			// Get all entities that have Transform & SpriteRenderer components
			(Transform[] transform,
			 SpriteRenderer[] renderers,
			 TestComponent[] testComponents)
			 = World.GetComponents<Transform, SpriteRenderer, TestComponent>();

			float t = (float)Math.Sin(Time.TimeSinceLaunch) + 0.1f;
			for (int i = 0; i < transform.Length; i++)
				renderers[i].Colour = Colour.Lerp(
					testComponents[i].Value,
					Colour.Black,
					t * testComponents[i].ValueChangeSpeed
				);

			if (Input.IsKeyPressed(Key.F11))
				Window.CycleFullscreen();

			if (Input.IsKeyDown(Key.LeftControl) && Input.IsKeyDown(Key.Q))
				Application.Exit();
		}
	}
}
