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

		private const float Padding = 20;
		private const float MinSettingsHeight = 125;

		public override void DrawInspector()
		{
			float aspectRatio = m_Target.Resolution.y / (float)m_Target.Resolution.x;
			IVector2 region = ImGUI.ContentRegionAvailable;
			float width = Math.Min(region.x, region.y - MinSettingsHeight);
			Vector2 textureSize = new Vector2(width - Padding, (width - Padding) * aspectRatio);

			ImGUI.BeginChild("TextureInspectorSettings",
						new Vector2(0, Math.Max(region.y - textureSize.y - Padding, MinSettingsHeight)));
			ImGUI.Checkbox("HDR", ref m_Settings.HDR);
			m_Settings.Filtering = ImGUI.EnumCombo("Filtering", m_Settings.Filtering);

			if(PendingChanges())
			{
				if (ImGUI.Button("Revert"))
					m_Settings = (TextureImportSettings)m_Target.ImportSettings;
				ImGUI.SameLine();
				if (ImGUI.Button("Apply"))
					m_Target.Import(m_Settings);
			}
			ImGUI.EndChild();

			string resolutionText = $"({m_Target.Resolution.x}x{m_Target.Resolution.y})";
			Vector2 resolutionTextSize = ImGUI.CalculateTextWidth(resolutionText);
			ImGUI.SetCursorPos(
				(region.x / 2.0f) - resolutionTextSize.x / 2.0f + Padding / 2,
				ImGUI.WindowContentRegionMax.y - textureSize.y - Padding / 2 - resolutionTextSize.y * 1.25f);
			ImGUI.Text(resolutionText, Colour.Grey);

			ImGUI.SetCursorPos(
				(region.x / 2.0f) - (textureSize.x / 2.0f) + Padding / 2,
				ImGUI.WindowContentRegionMax.y - textureSize.y - Padding / 2);
			ImGUI.Image(m_Target, textureSize);
		}

		private bool PendingChanges()
		{
			if (m_Settings.HDR != m_Target.HDR) return true;
			if (m_Settings.Filtering != m_Target.Filter) return true;
			return false;
		}
	}
}