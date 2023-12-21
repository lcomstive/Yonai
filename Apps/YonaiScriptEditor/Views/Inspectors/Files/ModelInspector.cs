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

		private Framebuffer m_PreviewFB;

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

		public override void Opened() =>
			m_PreviewFB = new Framebuffer(new FramebufferSpec
			{
				Attachments = new TextureFormat[] { TextureFormat.RGBA8 },
				Samples = 1,
				Resolution = new IVector2(400, 400)
			});

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
			ImGUI.BeginChild("ModelInspectorSettings", new Vector2(0, region.y - m_PreviewHeight - SplitterWidth * 2));
			{
				ImGUI.Text(m_Target.ResourcePath.Replace("project://Assets/", ""));

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

		private void DrawModelToPreview()
		{

		}
	}
}