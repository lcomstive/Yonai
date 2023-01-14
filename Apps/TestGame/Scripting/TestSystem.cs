using System;
using AquaEngine;
using ScriptingTest;

namespace TestGame
{
	public class TestSystem : AquaSystem
	{
		private static readonly Colour[] ColourValues =
		{
			Colour.White,
			Colour.Red,
			Colour.Green,
			Colour.Blue
		};

		protected override void Start()
		{
			TestComponent[] testComponents = World.GetComponents<TestComponent>();
			for(int i = 0; i < testComponents.Length; i++)
				testComponents[i].Value = ColourValues[i % ColourValues.Length];

			Log.Debug($"DLL Path Arg: " + Application.GetArg("dll-path"));
		}

		protected override void Update()
		{
			// Get all entities that have Transform & SpriteRenderer components
			(Transform[] transform,
			 SpriteRenderer[] renderers,
			 TestComponent[] testComponents)
			 = World.GetComponents<Transform, SpriteRenderer, TestComponent>();

			float t = (float)Math.Sin(Time.TimeSinceLaunch) + 0.1f;
			for(int i = 0; i < transform.Length; i++)
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
