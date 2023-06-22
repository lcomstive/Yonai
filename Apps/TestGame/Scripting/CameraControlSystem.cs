using System;
using AquaEngine;

namespace TestGame
{
	public class CameraControlSystem : AquaSystem
	{
		public const float ScrollSpeed = 100;
		public const float SideMoveSpeed = 1.0f;
		public const float DefaultFOV = 65.0f;

		public Vector2 MouseSensitivity = new Vector2(1, 1);

		private float xRot = 0, yRot = 0;
		private Camera m_Target = null;
		private Transform m_Transform = null;

		protected override void Enabled()
		{
			m_Target = Camera.Main;
			Log.Debug("CameraControlSystem target set to " + (m_Target ? m_Target.GetComponent<NameComponent>().Name : "(none)"));
		}

		protected override void Update()
		{
			if (!m_Target)
				return;
			if (!m_Transform)
				m_Transform = m_Target.GetComponent<Transform>();
			
			float scrollDelta = Input.ScrollDelta;
			m_Target.FOV -= scrollDelta * Time.DeltaTime * ScrollSpeed;
			m_Target.OrthographicSize -= scrollDelta * Time.DeltaTime * ScrollSpeed;

			Input.MouseState = Input.IsMouseDown(MouseButton.Right) ? MouseState.Disabled : MouseState.Normal;
			
			// Reset FOV
			if (Input.IsKeyPressed(Key.F5))
				m_Target.FOV = DefaultFOV;

			// Toggle orthographic
			if (Input.IsKeyPressed(Key.F6))
				m_Target.Orthographic = !m_Target.Orthographic;

			// Move to transform sides if middle mouse held
			if (Input.IsMouseDown(MouseButton.Middle))
			{
				Vector2 mouseDelta = Mouse.DeltaPosition * Time.DeltaTime * SideMoveSpeed;
				m_Transform.Position -= m_Transform.Right * mouseDelta.x;
				m_Transform.Position -= m_Transform.Up * mouseDelta.y;
			}
			
			// Reset camera rotation & FOV
			if (Input.IsKeyDown(Key.LeftControl) && Input.IsKeyPressed(Key.R))
			{
				m_Target.FOV = DefaultFOV;
				m_Transform.LocalRotation = Quaternion.Identity;
			}

			if (Input.IsKeyDown(Key.LeftControl) && Input.IsKeyDown(Key.G))
				m_Transform.LocalRotation = Quaternion.FromEuler(Vector3.Forward);

			m_Transform.LocalRotation = GetRotation();
		}

		private Quaternion GetRotation()
		{
			Vector3 euler = m_Transform.LocalRotation.Euler;

			if (Input.IsMouseDown(MouseButton.Right))
			{
				Vector2 mouseDelta = Input.MouseDelta / 1000.0f;
				xRot += mouseDelta.y * MouseSensitivity.x;
				yRot += mouseDelta.x * MouseSensitivity.y;
			}

			Quaternion pitch = Quaternion.AngleAxis(xRot, Vector3.Right);
			Quaternion yaw = Quaternion.AngleAxis(yRot, Vector3.Up);

			return (pitch * yaw).Normalised;
		}
	}
}
