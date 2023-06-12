using AquaEngine;
using AquaEngine.Graphics;

namespace AquaEditor.Views
{
	public class SceneView : View
	{
		private static World m_World = null;
		private static Camera m_Camera = null;
		private static RenderTexture m_Target = null;

		private bool m_IsFocused = false;
		private SceneViewCameraController m_Controller = null;

		// Gizmo Settings //
		private bool m_Snapping = false;
		private float m_SnappingAmount = 0.5f;
		private float m_SnappingAmountDegrees = 15.0f;
		private bool m_GizmoModeLocal = true;
		private ImGUI.ManipulateOperation m_GizmoMode = ImGUI.ManipulateOperation.Translate;

		private const float HeaderHeight = 30;

		[MenuItem("Window/Scene")]
		private static void MenuCallback() => EditorUIService.Open<SceneView>();

		protected override void Opened()
		{
			if (m_World)
				return; // Already created world

			m_World = World.Create("Scene View");

			m_Target = new RenderTexture(new IVector2(1920, 1080));

			Entity entity = m_World.CreateEntity();
			entity.AddComponent<NameComponent>().Name = "Scene Camera";
			m_Camera = entity.AddComponent<Camera>();
			m_Camera.RenderTarget = m_Target;

			// Transform
			Transform transform = entity.AddComponent<Transform>();
			transform.Position = new Vector3(0, 2, -15);
			// transform.Rotation = Quaternion.FromEuler(0, 180, 0);

			// Camera controller
			m_Controller = entity.AddComponent<SceneViewCameraController>();
			m_Controller.Start();
		}

		protected override void Closed()
		{
			m_Target.Dispose();
			m_Camera.RenderTarget = null;

			m_World.Destroy();

			m_World = null;
			m_Camera = null;
		}

		protected override void Draw()
		{
			bool isOpen = true;
			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowPadding, Vector2.Zero);
			ImGUI.PushStyleColour(ImGUI.StyleColour.WindowBg, Colour.Black);
			if (ImGUI.Begin("Scene", ref isOpen))
			{
				DrawHeader();

				// Store focus state. Update() exits early if this window not focused
				m_IsFocused = ImGUI.IsWindowFocused(ImGUI.FocusedFlags.RootAndChildWindows);

				IVector2 viewportSize = (IVector2)ImGUI.ContentRegionAvailable;

				// Draw to camera render target
				IRenderPipeline pipeline = Renderer.Pipeline;
				pipeline.Resolution = viewportSize;
				m_Target.Resolution = viewportSize;

				pipeline.Draw(m_Camera);

				// ImGUI.Image(pipeline.Output?.ColourAttachments[0] ?? null, viewportSize);
				ImGUI.Image(m_Target, viewportSize);

				DrawTransformGizmo();
			}
			ImGUI.End();
			ImGUI.PopStyleVar();
			ImGUI.PopStyleColour();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<SceneView>();
		}

		protected override void Update()
		{
			if (!m_IsFocused)
				return;

			m_Controller.Update();

			if (Input.IsKeyPressed(Key.G)) m_GizmoModeLocal = !m_GizmoModeLocal;

			if (Input.IsKeyPressed(Key.Z)) m_GizmoMode = ImGUI.ManipulateOperation.Translate;
			if (Input.IsKeyPressed(Key.X)) m_GizmoMode = ImGUI.ManipulateOperation.Rotate;
			if (Input.IsKeyPressed(Key.C)) m_GizmoMode = ImGUI.ManipulateOperation.Scale;
			if (Input.IsKeyPressed(Key.V)) m_GizmoMode = ImGUI.ManipulateOperation.Universal;
		}

		private void DrawTransformGizmo()
		{
			Transform target = (InspectorView.Target as Entity)?.GetComponent<Transform>();
			if (target == null) return;

			Vector2 pos = ImGUI.WindowPosition;
			Vector2 size = ImGUI.WindowContentRegionMax;
			ImGUI.Gizmo.SetDrawList();
			ImGUI.Gizmo.SetRect(pos, size);

			float snapping = 0;
			if (m_Snapping && m_GizmoMode == ImGUI.ManipulateOperation.Rotate)
				snapping = m_SnappingAmountDegrees;
			else if(m_Snapping)
				snapping = m_SnappingAmount;
			ImGUI.Gizmo.Manipulate(m_Camera, target, size, m_GizmoMode, m_GizmoModeLocal, snapping);
		}

		private void DrawHeaderOperationSelector(string label, ImGUI.ManipulateOperation operation)
		{
			if (m_GizmoMode == operation)
				ImGUI.BeginDisabled();

			if (ImGUI.Button(label))
				m_GizmoMode = operation;

			if (m_GizmoMode == operation)
				ImGUI.EndDisabled();

			ImGUI.SameLine();
		}

		private void DrawHeader()
		{
			ImGUI.PushStyleVar(ImGUI.StyleVar.ChildRounding, 0);
			ImGUI.PushStyleColour(ImGUI.StyleColour.ChildBg, new Colour(0, 0, 0, 0.2f));
			ImGUI.BeginChild("SceneViewHeader", new Vector2(0, HeaderHeight));
			{
				DrawHeaderOperationSelector("T", ImGUI.ManipulateOperation.Translate);
				DrawHeaderOperationSelector("R", ImGUI.ManipulateOperation.Rotate);
				DrawHeaderOperationSelector("S", ImGUI.ManipulateOperation.Scale);
				DrawHeaderOperationSelector("U", ImGUI.ManipulateOperation.Universal);

				ImGUI.HorizontalSpace(10);

				if (ImGUI.Button(m_GizmoModeLocal ? "Local" : "Global"))
					m_GizmoModeLocal = !m_GizmoModeLocal;
				ImGUI.SameLine();

				ImGUI.HorizontalSpace(10);

				ImGUI.Checkbox("Snapping", ref m_Snapping);
			}
			ImGUI.EndChild();
			ImGUI.PopStyleColour();
			ImGUI.PopStyleVar();

			ImGUI.SetCursorPos(Vector2.Zero);
		}
	}
}