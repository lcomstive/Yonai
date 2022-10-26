using System;
using System.Diagnostics;
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

		protected override void OnEnabled() => Log.Debug("TestComponent.OnEnabled");
		protected override void OnDisabled() => Log.Debug("TestComponent.OnDisabled");
		protected override void OnDestroyed() => Log.Debug("TestComponent.OnDestroyed");
	}

	public class HelloWorld
	{
		private TestComponent m_TestComponent;

		public void Start()
		{
			Log.Debug("Native logging test");
			Log.Info("Native logging test");
			Log.Warning("Native logging test");
			Log.Error("Native logging test");

			#region Vector3 Test
			Vector3 v3a = new Vector3(4, 2, 0);
			Vector3 v3b = new Vector3(3, 6, 2);
			Log.Info($"Vector3 A: {v3a}");
			Log.Info($"Vector3 B: {v3b}");
			Log.Info($"Vector3 Addition: {v3a + v3b}");
			Log.Info($"Vector3 Multiplication: {v3a * v3b}");
			Log.Info($"Vector3 Dot: {Vector3.Dot(v3a, v3b)}");
			Log.Info($"Vector3 Distance: " + Vector3.Distance(v3a, v3b));
			#endregion

			#region Vector2 Test
			Vector2 v2a = new Vector2(6, 9);
			Vector2 v2b = new Vector2(3, 6);
			Log.Info("Vector2 A: " + v2a);
			Log.Info("Vector2 B: " + v2b);
			Log.Info("Vector2 Addition: " + (v2a + v2b));
			Log.Info("Vector2 Multiplication: " + (v2a * v2b));
			Log.Info("Vector2 Distance: " + Vector2.Distance(v2a, v2b));
			#endregion

			Log.Critical("Native logging test");

			World world = World.Get(0);
			Log.Debug($"World loaded: '{world.Name}'");

			Entity entity = world.GetEntity(0);
			m_TestComponent = entity.AddComponent<TestComponent>();
			if (m_TestComponent != null)
				Log.Debug("Added test component");
			else
				Log.Error("TestComponent not created?");
		}
	}
}
