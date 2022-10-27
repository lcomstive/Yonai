using System;
using AquaEngine;

namespace ScriptingTest
{
	public class TestComponent : Component
	{
		public Transform Target = null;
		public float Speed = 2.0f;
		public float Length = 0.5f;

		TestComponent() => Log.Debug("TestComponent.Constructor");

		protected override void Start()
		{
			Log.Debug("TestComponent.Start");

			Target = Entity.GetComponent<Transform>();
			Log.Debug("Target found? " + (Target != null ? "Yes" : "No"));

			Log.Debug($"Speed: {Speed}");
			Log.Debug($"Length: {Length}");
		}

		protected override void Update()
		{
			if (Target == null)
				return;

			Target.Position = Vector3.Up * (float)Math.Sin(Time.TimeSinceLaunch * Speed) * Length;
			Target.Position -= Vector3.Forward;
		}

		protected override void Enabled() => Log.Debug("TestComponent.OnEnabled");
		protected override void Disabled() => Log.Debug("TestComponent.OnDisabled");
		protected override void Destroyed() => Log.Debug("TestComponent.OnDestroyed");
	}
}
