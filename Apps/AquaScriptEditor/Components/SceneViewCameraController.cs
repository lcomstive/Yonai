using AquaEngine;
using System;

namespace AquaEditor
{
	public class SceneViewCameraController : Component
	{
		public float Speed = 1.0f;
		public float SprintMultiplier = 2.5f;

		private Transform m_Transform;

		private const float ControllerDeadzone = 0.1f;

		protected override void Start() => m_Transform = GetComponent<Transform>();

		protected override void Update()
		{
			Vector3 movement = GetMovement();
			Quaternion rotation = GetRotation();

			// Apply speed to movement
			if (Input.IsKeyDown(Key.LeftShift))
				movement *= SprintMultiplier;
			else
				movement *= Speed;

			m_Transform.Position += m_Transform.Rotation.Inverted * movement;
			m_Transform.Rotation = rotation;
		}

		private Vector3 GetMovement()
		{
			Vector3 movement = Vector3.Zero;

			if (Input.IsKeyDown(Key.W)) movement.z++;
			if (Input.IsKeyDown(Key.S)) movement.z--;
			if (Input.IsKeyDown(Key.A)) movement.x++;
			if (Input.IsKeyDown(Key.D)) movement.x--;
			if (Input.IsKeyDown(Key.E)) movement.y++;
			if (Input.IsKeyDown(Key.Q)) movement.y--;
			
			if(Input.IsGamepadConnected(0))
			{
				Vector2 gamepadInput = new Vector2(
					Input.GetAxis(0, GamepadAxis.AxisLeftX),
					Input.GetAxis(0, GamepadAxis.AxisLeftY)
				);

				if (gamepadInput.Magnitude >= ControllerDeadzone)
					movement += new Vector3(gamepadInput.x, 0, gamepadInput.y);
			}

			return movement * Time.DeltaTime;
		}

		private float xRot = 0, yRot = 0;
		private Quaternion GetRotation()
		{
			Input.MouseState = Input.IsMouseDown(MouseButton.Right) ? MouseState.Disabled : MouseState.Normal;
			if(Input.IsMouseDown(MouseButton.Right))
			{
				Vector2 mouseDelta = Input.MouseDelta * Time.DeltaTime;
				xRot += mouseDelta.y;
				yRot += mouseDelta.x;
			}

			Quaternion pitch = Quaternion.AngleAxis(xRot, Vector3.Right);
			Quaternion yaw	 = Quaternion.AngleAxis(yRot, Vector3.Up);

			return (pitch * yaw).Normalised;
		}
	}
}