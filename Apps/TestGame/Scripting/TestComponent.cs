using System;
using AquaEngine;

namespace ScriptingTest
{
	public class TestComponent : Component
	{
		public Transform Target = null;
		public float Speed = 2.5f;
		public float Length = 0.5f;

		public const float ScrollSpeed = 1.25f;

		protected override void Start() => Target = GetComponent<Transform>();

		protected override void Update()
		{
			Target = GetComponent<Transform>();
			if (Target == null)
				return;

			Camera mainCam = Camera.Main;
			UpdateCamera(mainCam);

			float scale = (float)Math.Cos(Time.TimeSinceLaunch * Speed + Target.Position.x + Target.Position.y);
			scale = (scale / 2.0f) + 0.5f;
			scale *= Length;
			Target.Scale = new Vector3(scale);

			// Move to transform sides if middle mouse held
			if(Input.IsMouseDown(MouseButton.Middle))
			{
				Vector2 mouseDelta = Mouse.DeltaPosition * Time.DeltaTime * .01f;
				Transform camTransform = mainCam.GetComponent<Transform>();
				camTransform.Position -= camTransform.Right * mouseDelta.x;
				camTransform.Position -= camTransform.Up * mouseDelta.y;
			}
		}

		private void UpdateCamera(Camera camera)
		{
			if (!camera)
				return;

			float scrollDelta = Input.ScrollDelta;
			camera.FOV -= scrollDelta * Time.DeltaTime * ScrollSpeed;
			camera.OrthographicSize -= scrollDelta * Time.DeltaTime * ScrollSpeed;

			// Reset FOV
			if (Input.IsKeyPressed(Key.F5))
				camera.FOV = 65.0f;

			// Toggle orthographic
			if (Input.IsKeyPressed(Key.F6))
				camera.Orthographic = !camera.Orthographic;
		}
	}
}
