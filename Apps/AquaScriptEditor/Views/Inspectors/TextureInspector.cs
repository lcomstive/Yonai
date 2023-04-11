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

		public override void DrawInspector()
		{
			ImGUI.Image(m_Target, ImGUI.ContentRegionAvailable);
		}
	}
}