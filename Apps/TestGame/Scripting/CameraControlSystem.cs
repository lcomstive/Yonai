using Yonai;

namespace TestGame
{
	public class CameraControlSystem : YonaiSystem
	{
		public const float ScrollSpeed = 100;
		public const float SideMoveSpeed = 1.0f;
		public const float DefaultFOV = 65.0f;

		public float MoveSpeed = 5;
		public Vector2 MouseSensitivity = new Vector2(1, 1);

		private float xRot = 0, yRot = 0;
		private Camera m_Target = null;
		private Transform m_Transform = null;

		protected override void Enabled()
		{
			m_Target = Camera.Main;
			// Input.MouseState = MouseState.Disabled;
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

			if(Input.IsKeyPressed(Key.Escape))
				Input.MouseState = Input.MouseState == MouseState.Disabled ? MouseState.Normal : MouseState.Disabled;
			
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
			m_Transform.LocalPosition += m_Transform.LocalRotation.Inverted * GetMovement();
		}

		private Quaternion GetRotation()
		{
			Vector2 mouseDelta = Input.MouseDelta / 1000.0f;
			xRot += mouseDelta.y * MouseSensitivity.x;
			yRot += mouseDelta.x * MouseSensitivity.y;

			Quaternion pitch = Quaternion.AngleAxis(xRot, Vector3.Right);
			Quaternion yaw = Quaternion.AngleAxis(yRot, Vector3.Up);

			return (pitch * yaw).Normalised;
		}

		private const float MovementInputLerpSpeed = 10.0f;
		private Vector3 m_PreviousMovement = Vector3.Zero;
		private Vector3 GetMovement()
		{
			Vector3 movement = Vector3.Zero;
			if (Input.IsKeyDown(Key.W)) movement.z++;
			if (Input.IsKeyDown(Key.S)) movement.z--;
			if (Input.IsKeyDown(Key.A)) movement.x++;
			if (Input.IsKeyDown(Key.D)) movement.x--;
			if (Input.IsKeyDown(Key.E)) movement.y++;
			if (Input.IsKeyDown(Key.Q)) movement.y--;

			movement.z += Input.GetAxis(0, GamepadAxis.AxisLeftY);
			movement.x += Input.GetAxis(0, GamepadAxis.AxisLeftX);

			movement = Vector3.Lerp(m_PreviousMovement, movement, MovementInputLerpSpeed * Time.DeltaTime);
			m_PreviousMovement = movement;

			return movement * MoveSpeed * Time.DeltaTime;
		}
	}
}
