using System;
using AquaEngine;
using TestGame;

namespace ScriptingTest
{
	public class TestComponent : Component
	{
		protected override void Start()
		{
			if (!World.HasSystem<TransformScaleSystem>())
			{
				Log.Info("Adding transform scale system...");
				World.AddSystem<TransformScaleSystem>();
			}
		}
	}
}
