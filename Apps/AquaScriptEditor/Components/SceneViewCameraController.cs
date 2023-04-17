using AquaEngine;

namespace AquaEditor
{
	public class SceneViewCameraController : Behaviour
	{
		public float Speed = 1.0f;
		public float SprintMultiplier = 2.5f;
		public Vector2 MouseSensitivity = new Vector2(1, 1);

		private Transform m_Transform;
		private float xRot = 0, yRot = 0;

		private const float ControllerDeadzone = 0.1f;

		public override void Start()
		{
			m_Transform = GetComponent<Transform>();

			if (!m_Transform)
			{
				Log.Warning("Transform empty??");
				return;
			}
			else
				Log.Warning("Transform found");

			Vector3 euler = m_Transform.LocalRotation.Euler;
			yRot = MathUtils.Deg2Rad(euler.y);
			xRot = MathUtils.Deg2Rad(euler.x);
		}

		public override void Update()
		{
			Vector3 movement = GetMovement();
			Quaternion rotation = GetRotation();

			// Apply speed to movement
			if (Input.IsKeyDown(Key.LeftShift))
				movement *= SprintMultiplier;
			else
				movement *= Speed;

			m_Transform.LocalPosition += m_Transform.LocalRotation.Inverted * movement;
			m_Transform.LocalRotation = rotation;
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

		private Quaternion GetRotation()
		{
			Vector3 euler = m_Transform.LocalRotation.Euler;

			Input.MouseState = Input.IsMouseDown(MouseButton.Right) ? MouseState.Disabled : MouseState.Normal;
			if(Input.IsMouseDown(MouseButton.Right))
			{
				Vector2 mouseDelta = Input.MouseDelta / 1000.0f;
				xRot += mouseDelta.y * MouseSensitivity.x;
				yRot += mouseDelta.x * MouseSensitivity.y;
			}

			Quaternion pitch = Quaternion.AngleAxis(xRot, Vector3.Right);
			Quaternion yaw	 = Quaternion.AngleAxis(yRot, Vector3.Up);

			return (pitch * yaw).Normalised;
		}
	}
}