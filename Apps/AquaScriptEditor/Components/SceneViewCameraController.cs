using AquaEngine;

namespace AquaEditor
{
	public class SceneViewCameraController : Component
	{
		public float Speed = 1.0f;
		public float SprintMultiplier = 2.5f;
		public Vector2 MouseSensitivity = new Vector2(1, 1);

		private Transform m_Transform;
		private float xRot = 0, yRot = 0;

		private const float ControllerDeadzone = 0.1f;

		public void Start()
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

		public void Update()
		{
			Vector3 movement = GetMovement();
			Quaternion rotation = GetRotation();

			// Apply speed to movement
			if (ImGUI.IsKeyDown(Key.LeftShift))
				movement *= SprintMultiplier;
			else
				movement *= Speed;

			m_Transform.LocalPosition += m_Transform.LocalRotation.Inverted * movement;
			m_Transform.LocalRotation = rotation;
		}

		private Vector3 GetMovement()
		{
			Vector3 movement = Vector3.Zero;

			if (ImGUI.IsKeyDown(Key.W)) movement.z++;
			if (ImGUI.IsKeyDown(Key.S)) movement.z--;
			if (ImGUI.IsKeyDown(Key.A)) movement.x++;
			if (ImGUI.IsKeyDown(Key.D)) movement.x--;
			if (ImGUI.IsKeyDown(Key.E)) movement.y++;
			if (ImGUI.IsKeyDown(Key.Q)) movement.y--;

			return movement * Time.DeltaTime;
		}

		private Quaternion GetRotation()
		{
			Vector3 euler = m_Transform.LocalRotation.Euler;

			ImGUI.SetMouseCursor(ImGUI.IsMouseDragging(MouseButton.Right) ? ImGUI.MouseCursor.None : ImGUI.MouseCursor.Arrow);
			if(ImGUI.IsMouseDragging(MouseButton.Right))
			{
				Vector2 mouseDelta = ImGUI.GetMouseDelta() / 1000.0f;
				xRot += mouseDelta.y * MouseSensitivity.x;
				yRot += mouseDelta.x * MouseSensitivity.y;
			}

			Quaternion pitch = Quaternion.AngleAxis(xRot, Vector3.Right);
			Quaternion yaw	 = Quaternion.AngleAxis(yRot, Vector3.Up);

			return (pitch * yaw).Normalised;
		}
	}
}