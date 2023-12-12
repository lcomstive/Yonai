using System;

namespace Yonai
{
	public class Camera : Component, IDisposable
	{
		/// <summary>
		/// The camera that is targeted for rendering to the display
		/// </summary>
		public static Camera Main { get; set; } = null;

		/// <summary>
		/// Field of view
		/// </summary>
		public float FOV { get; set; }

		/// <summary>
		/// Near clipping plane
		/// </summary>
		public float Near { get; set; }

		/// <summary>
		/// Far clipping plane
		/// </summary>
		public float Far { get; set; }

		/// <summary>
		/// Is the camera in orthographic mode, or perspective?
		/// </summary>
		public bool Orthographic { get; set; }

		/// <summary>
		/// Size of the orthographic view
		/// </summary>
		public float OrthographicSize { get; set; }

		public Matrix4 ViewMatrix
		{
			get
			{
				Transform transform = GetComponent<Transform>();
				if (!transform) return Matrix4.Identity;

				return Matrix4.LookAt(
					transform.Position,
					transform.Position + transform.Forward,
					transform.Up
				);
			}
		}

		public Camera()
		{
			World.StateChanged += OnWorldStateChanged;
			OnWorldStateChanged(World.IsActive);
		}

		public void Dispose()
		{
			// If this is the main camera, clear
			if (Main == this)
				Main = null;

			World.StateChanged -= OnWorldStateChanged;
		}

		private void OnWorldStateChanged(bool active)
		{
			// If world becomes active and no camera set to main, this becomes main
			if (active && !Main)
				Main = this;
			// If this is the main camera but world is deactivating, clear main
			else if (!active && Main == this)
				Main = null;
		}

		public Matrix4 GetProjectionMatrix(IVector2 resolution) => GetProjectionMatrix(resolution.x, resolution.y);
		public Matrix4 GetProjectionMatrix(int width, int height) =>
			Matrix4.Perspective(FOV, width / (float)height, Near, Far);
	}
}
