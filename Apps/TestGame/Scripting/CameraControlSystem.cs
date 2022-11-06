using System;
using AquaEngine;

namespace TestGame
{
	public class CameraControlSystem : AquaEngine.System
	{
		public const float ScrollSpeed = 100;
		public const float SideMoveSpeed = 1.0f;
		
		public Camera m_Target = null;

		protected override void Start() => m_Target = Camera.Main;

		protected override void Update()
		{
			float scrollDelta = Input.ScrollDelta;
			m_Target.FOV -= scrollDelta * Time.DeltaTime * ScrollSpeed;
			m_Target.OrthographicSize -= scrollDelta * Time.DeltaTime * ScrollSpeed;
			
			// Reset FOV
			if (Input.IsKeyPressed(Key.F5))
				m_Target.FOV = 65.0f;

			// Toggle orthographic
			if (Input.IsKeyPressed(Key.F6))
				m_Target.Orthographic = !m_Target.Orthographic;

			// Move to transform sides if middle mouse held
			if (Input.IsMouseDown(MouseButton.Middle))
			{
				Vector2 mouseDelta = Mouse.DeltaPosition * Time.DeltaTime * SideMoveSpeed;
				Transform camTransform = m_Target.GetComponent<Transform>();
				camTransform.Position -= camTransform.Right * mouseDelta.x;
				camTransform.Position -= camTransform.Up * mouseDelta.y;
			}
		}
	}
}
