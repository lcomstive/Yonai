using AquaEngine;
using System.Collections.Generic;

namespace AquaEditor.Views
{
	public class SceneView : View
	{
		private World m_World = null;
		private Camera m_Camera = null;
		private Transform m_Transform = null;

		[MenuItem("Window/Scene")]
		private static void MenuCallback() => EditorUIService.Open<SceneView>();

		protected override void Opened()
		{
			m_World = World.Create("Scene View");

			Entity entity = m_World.CreateEntity();
			m_Camera = entity.AddComponent<Camera>();
			m_Transform = entity.AddComponent<Transform>();
			entity.AddComponent<SceneViewCameraController>();

			m_Transform.Position = new Vector3(0, 10, -10);
			m_Transform.Rotation = Quaternion.FromEuler(-45, 0, 0);
		}

		protected override void Closed()
		{
			m_World.Destroy();

			m_World = null;
			m_Camera = null;
			m_Transform = null;
		}

		protected override void Draw()
		{
			bool isOpen = true;
			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowPadding, Vector2.Zero);
			if (ImGUI.Begin("Scene", ref isOpen))
			{
				Vector2 offset = ImGUI.WindowPosition;

				Vector2[] viewportBounds = new Vector2[]
				{
					ImGUI.WindowContentRegionMin + offset,
					ImGUI.WindowContentRegionMax + offset
				};

				// Disable input system to game content unless this view is focused
				Input.Enabled = ImGUI.IsWindowFocused;

				
			}
			ImGUI.End();
			ImGUI.PopStyleVar();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<SceneView>();
		}
	}
}