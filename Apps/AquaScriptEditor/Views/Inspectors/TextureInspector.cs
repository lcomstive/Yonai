using System;
using AquaEngine;
using AquaEngine.Graphics;

namespace AquaEditor.Inspectors
{
	[CustomInspector(typeof(Texture), ".png", ".jpg", ".jpeg", ".dds")]
	public class TextureInspector : CustomInspector
	{
		private Texture m_Target;
		private TextureImportSettings m_Settings;

		public override void OnTargetChanged()
		{
			// Check if filepath
			if(Target is VFSFile)
			{
				VFSFile file = (VFSFile)Target;
				string resourcePath = file.FullPath.Replace("project://Assets/", "Textures/").Replace(file.Extension, "");
				m_Target = Resource.Load<Texture>(resourcePath, new TextureImportSettings(file.FullPath));
			}
			else // Is Texture object
				m_Target = Target as Texture;
			m_Settings = (TextureImportSettings)m_Target.ImportSettings;
		}

		private float m_TexturePreviewHeight = -1;
		private const float TexturePreviewMinHeight = 10;
		private const float SplitterWidth = 7.5f;
		public override void DrawInspector()
		{
			IVector2 region = ImGUI.ContentRegionAvailable;

			if (m_TexturePreviewHeight < 0)
				m_TexturePreviewHeight = region.y * 0.4f;

			// Import settings //
			ImGUI.BeginChild("TextureInspectorSettings", new Vector2(0, region.y - m_TexturePreviewHeight - SplitterWidth * 2));
			{
				ImGUI.Text(m_Target.FilePath.Replace("project://Assets/", ""));
				ImGUI.SameLine();
				ImGUI.Text($"({m_Target.Resolution.x}x{m_Target.Resolution.y})", Colour.Grey);

				ImGUI.Space();
				ImGUI.Separator();
				ImGUI.Space();

				ImGUI.Checkbox("HDR", ref m_Settings.HDR);
				m_Settings.Filtering = ImGUI.EnumCombo("Filtering", m_Settings.Filtering);

				if (PendingChanges())
				{
					if (ImGUI.Button("Revert"))
						m_Settings = (TextureImportSettings)m_Target.ImportSettings;
					ImGUI.SameLine();
					if (ImGUI.Button("Apply"))
						m_Target.Import(m_Settings);
				}
			}
			ImGUI.EndChild();

			ImGUI.InvisibleButton("TextureInspectorTextureSplitter", new Vector2(-1, SplitterWidth));
			if (ImGUI.IsItemHovered()) ImGUI.SetMouseCursor(ImGUI.MouseCursor.ResizeNS);
			if (ImGUI.IsItemActive()) m_TexturePreviewHeight = MathUtils.Clamp(m_TexturePreviewHeight - ImGUI.GetMouseDeltaY(), TexturePreviewMinHeight, region.y - TexturePreviewMinHeight);

			ImGUI.PushStyleVar(ImGUI.StyleVar.FramePadding, Vector2.Zero);
			ImGUI.PushStyleColour(ImGUI.StyleColour.ChildBg, Colour.Black);
			ImGUI.BeginChild("TextureInspectorPreview", new Vector2(0, m_TexturePreviewHeight));
			{
				float aspectRatio = m_Target.Resolution.y / (float)m_Target.Resolution.x;
				float width = Math.Min(region.x, m_TexturePreviewHeight);
				ImGUI.SetCursorPos(region.x / 2.0f - width / 2, 0);
				ImGUI.Image(m_Target, new Vector2(width, width * aspectRatio));
			}
			ImGUI.EndChild();
			ImGUI.PopStyleVar();
			ImGUI.PopStyleColour();
		}

		private bool PendingChanges()
		{
			if (m_Settings.HDR != m_Target.HDR) return true;
			if (m_Settings.Filtering != m_Target.Filter) return true;
			return false;
		}
	}
}