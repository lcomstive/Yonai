using System;
using AquaEngine;

namespace ScriptingTest
{
	public class TestComponent : Component
	{
		public Transform Target = null;
		public float Speed = 2.5f;
		public float Length = 0.5f;

		TestComponent() => Log.Debug("TestComponent.Constructor");

		protected override void Start()
		{
			Log.Debug("TestComponent.Start");

			Target = Entity.GetComponent<Transform>();

			Log.Debug($"Speed: {Speed}");
			Log.Debug($"Length: {Length}");
		}

		protected override void Update()
		{
			if (Target == null)
				return;

			float scale = (float)Math.Cos(Time.TimeSinceLaunch * Speed + Target.Position.x + Target.Position.y);
			scale = (scale / 2.0f) + 0.5f;
			scale *= Length;
			Target.Scale = new Vector3(scale);
		}

		protected override void Enabled() => Log.Debug("TestComponent.OnEnabled");
		protected override void Disabled() => Log.Debug("TestComponent.OnDisabled");
		protected override void Destroyed() => Log.Debug("TestComponent.OnDestroyed");
	}
}
