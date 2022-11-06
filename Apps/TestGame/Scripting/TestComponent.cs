using System;
using AquaEngine;
using TestGame;

namespace ScriptingTest
{
	public class TestComponent : Component
	{
		private SpriteRenderer m_SpriteRenderer;

		protected override void Start() => m_SpriteRenderer = GetComponent<SpriteRenderer>();

		protected override void Update()
		{
			float time = Time.TimeSinceLaunch;
			float cos = (float)Math.Cos(time);
			float sin = (float)Math.Sin(time);
			m_SpriteRenderer.Colour = new Colour(cos, sin, cos + sin);
		}
	}

}
