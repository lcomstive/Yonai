using System;
using Yonai;
using Yonai.IO;
using Yonai.Graphics;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(
		typeof(Model),
		".obj",
		".fbx",
		".blend",
		".3ds",
		".gltf"
	)]
	public class ModelInspector : CustomInspector
	{
		private Model m_Target;
		private ModelImportSettings m_Settings;

		// Preview
		private Framebuffer m_PreviewFB;
		private Material m_DefaultPreviewMaterial;
		private World m_PreviewWorld;
		private Camera m_PreviewCamera;
		private Transform m_PreviewTransform;

		public override void OnTargetChanged()
		{
			// Check if filepath
			if (Target is VFSFile)
			{
				VFSFile file = (VFSFile)Target;
				m_Target = Resource.Load<Model>(file.FullPath);
			}
			else // Is object instance
				m_Target = Target as Model;
			m_Settings = (ModelImportSettings)m_Target.ImportSettings;
		}

		public override void Opened()
		{
			m_PreviewFB = new Framebuffer(new FramebufferSpec
			{
				Attachments = new TextureFormat[] { TextureFormat.RGBA8 },
				Samples = 1,
				Resolution = new IVector2(400, 400)
			});

			m_DefaultPreviewMaterial = Resource.Get<Material>("assets://Materials/Default3D.material");

			m_PreviewWorld = Resource.Load<World>("editor://ModelPreview.world", false);
			Entity camera = m_PreviewWorld.CreateEntity();
			m_PreviewCamera = camera.AddComponent<Camera>();
			camera.AddComponent<Transform>().Position = new Vector3(0, 0, -10);

			m_PreviewTransform = m_PreviewWorld.CreateEntity().AddComponent<Transform>();
			m_PreviewTransform.Position = new Vector3(0, 0, 10);
		}

		public override void Closed() => m_PreviewFB?.Dispose();

		private int m_PreviewHeight = -1;
		private const float ModelPreviewMinHeight = 10;
		private const float SplitterWidth = 7.5f;
		public override void DrawInspector()
		{
			Vector2 region = ImGUI.ContentRegionAvailable;

			if (m_PreviewHeight < 0)
			{
				m_PreviewHeight = (int)Math.Floor(region.y * 0.5f);
				m_PreviewFB.Resolution = new IVector2((int)region.x, m_PreviewHeight);
			}

			// Import settings //
			ImGUI.BeginChild("ModelInspectorSettings", new Vector2(0, region.y - m_PreviewHeight - SplitterWidth * 2 - 10));
			{
				ImGUI.Text(m_Target.ResourcePath.FullPath.Replace("project://Assets/", ""));

				ImGUI.Space();
				ImGUI.Separator();
				ImGUI.Space();

				ImGUI.Checkbox("Import Materials", ref m_Settings.ImportMaterials);

				if (PendingChanges())
				{
					if (ImGUI.Button("Revert"))
						m_Settings = (ModelImportSettings)m_Target.ImportSettings;
					ImGUI.SameLine();
					if (ImGUI.Button("Apply"))
					{
						m_Target.Import(m_Settings);
						Resource.SaveToDisk(m_Target);
					}
				}
			}
			ImGUI.EndChild();

			ImGUI.InvisibleButton("ModelInspectorPreviewSplitter", new Vector2(-1, SplitterWidth));
			if (ImGUI.IsItemHovered()) ImGUI.SetMouseCursor(ImGUI.MouseCursor.ResizeNS);
			if (ImGUI.IsItemActive()) m_PreviewHeight = (int)MathUtils.Clamp(m_PreviewHeight - ImGUI.GetMouseDeltaY(), ModelPreviewMinHeight, region.y - ModelPreviewMinHeight);
			if (ImGUI.IsItemHovered() && ImGUI.IsMouseReleased(MouseButton.Left))
				m_PreviewFB.Resolution = new IVector2((int)region.x, m_PreviewHeight);

			// Preview of model
			DrawModelToPreview();
			ImGUI.Image(m_PreviewFB.GetColourAttachment(), m_PreviewFB.Resolution);
		}

		private bool PendingChanges()
		{
			if (m_Settings.ImportMaterials != m_Target.ImportMaterials) return true;
			return false;
		}

		private Vector3 m_ModelRotation = Vector3.Zero;
		private const float ScrollSpeed = 3;
		private const float ZoomSpeed = 5000;
		private float m_DesiredFOV = 90;
		private void DrawModelToPreview()
		{
			m_PreviewFB.Bind();
			Renderer.ClearColour(Colour.Black);
			Renderer.Clear(BufferFlags.Colour | BufferFlags.Depth);
			Renderer.Enable(RenderFeature.DepthTest);
			Renderer.CullFace(CullFace.Back);

			#region Controls
			if (ImGUI.IsMouseDragging(MouseButton.Left))
			{
				m_ModelRotation.y += ImGUI.GetMouseDeltaX();
				m_ModelRotation.x += ImGUI.GetMouseDeltaY();
			}

			m_PreviewTransform.LocalRotation = Quaternion.FromEuler(m_ModelRotation);

			if (ImGUI.IsWindowHovered())
			{
				// m_PreviewCamera.FOV = MathUtils.Clamp(m_PreviewCamera.FOV - ImGUI.GetMouseScrollDelta() * ScrollSpeed, 10, 120);
				m_DesiredFOV = MathUtils.Clamp(m_DesiredFOV - ImGUI.GetMouseScrollDelta() * ScrollSpeed, 10, 120);
				m_PreviewCamera.FOV = MathUtils.Lerp(m_PreviewCamera.FOV, m_DesiredFOV, Time.DeltaTime * ZoomSpeed);
			}
			#endregion

			foreach (Model.MeshData mesh in m_Target.Meshes)
			{
				Material material = mesh.Material ?? m_DefaultPreviewMaterial;
				Shader shader = material.Shader ?? m_DefaultPreviewMaterial.Shader;

				shader.Bind();
				shader.Set("resolution", m_PreviewFB.Resolution);

				Renderer.UploadCamera(shader, m_PreviewCamera, m_PreviewFB.Resolution);
				Renderer.UploadTransform(shader, m_PreviewTransform);
				Renderer.DrawMesh(mesh.Mesh);

				shader.Unbind();
			}

			m_PreviewFB.Unbind();
		}
	}
}