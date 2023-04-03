using AquaEngine;
using AquaEngine.Graphics;

namespace AquaEditor.Views
{
	public class SceneView : View
	{
		private static World m_World = null;
		private static Camera m_Camera = null;
		private static RenderTexture m_Target = null;

		[MenuItem("Window/Scene")]
		private static void MenuCallback() => EditorUIService.Open<SceneView>();

		protected override void Opened()
		{
			m_World = World.Create("Scene View");

			m_Target = new RenderTexture(new IVector2(1920, 1080));

			Entity entity = m_World.CreateEntity();
			entity.AddComponent<NameComponent>().Name = "Scene Camera";
			m_Camera = entity.AddComponent<Camera>();
			m_Camera.RenderTarget = m_Target;

			// Camera controller
			entity.AddComponent<SceneViewCameraController>();

			// Transform
			Transform transform = entity.AddComponent<Transform>();
			transform.Position = new Vector3(0, 2, -15);
			transform.Rotation = Quaternion.FromEuler(-45, 0, 0);

			SceneManager.Load(m_World, SceneAddType.Additive);

			InspectorView.Target = entity;
		}

		protected override void Closed()
		{
			SceneManager.Unload(m_World);
			m_World.Destroy();

			m_World = null;
			m_Camera = null;
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

				IVector2 viewportSize = ImGUI.ContentRegionAvailable;

				// Draw to camera render target
				IRenderPipeline pipeline = Renderer.Pipeline;
				pipeline.Resolution = viewportSize;

				m_Target.Resolution = viewportSize;
				pipeline.Draw(m_Camera);

				ImGUI.Image(pipeline.Output?.ColourAttachments[0] ?? null, viewportSize);
			}
			ImGUI.End();
			ImGUI.PopStyleVar();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<SceneView>();
		}
	}
}