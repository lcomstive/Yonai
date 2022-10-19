using System;
using AquaEngine;

namespace ScriptingTest
{
	public class HelloWorld
	{
		public float TestFloat = 5.0f;

		Transform transform = null;

		public void PrintTestFloat() => Console.WriteLine($"Test float: {TestFloat}");

		public void PrintTestMsg()
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

			// Get transform of the first entity created, in the first available World
			transform = new Transform(new Entity(new World(0), 0));
		}

		public void Update()
		{
			transform.Position = Vector3.Right * (float)Math.Sin(Time.TimeSinceLaunch);
		}
	}
}
