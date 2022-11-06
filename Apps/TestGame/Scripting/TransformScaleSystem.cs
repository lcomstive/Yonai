using System;
using AquaEngine;
using System.Collections.Generic;

namespace TestGame
{
	public class TransformScaleSystem : AquaEngine.System
	{
		public float Speed = 2.5f;
		public float Length = 0.5f;

		private List<Entity> m_Entities = new List<Entity>();

		protected override void Start()
		{
			m_Entities.Clear();

			uint entityID = 0;
			while(World.HasEntity(entityID))
				m_Entities.Add(World.GetEntity(entityID++));

			Log.Debug($"Found {m_Entities.Count} entities");
		}

		protected override void Update()
		{
			// Log.Debug("TransformScaleSystem.Update()");

			foreach (Entity entity in m_Entities)
			{
				Transform transform = entity.GetComponent<Transform>();

				float scale = (float)Math.Cos(Time.TimeSinceLaunch * Speed + transform.Position.x + transform.Position.y);
				scale = (scale / 2.0f) + 0.5f;
				scale *= Length;
				transform.Scale = new Vector3(scale);
			}
		}
	}
}
