using Yonai;
using Yonai.IO;
using Yonai.Graphics;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(Material), ".material")]
	public class MaterialInspector : CustomInspector
	{
		private Material m_Target;
		private MaterialImportSettings m_Settings;

		public override void OnTargetChanged()
		{
			// Check if filepath is selected (resource view)
			if (Target is VFSFile)
			{
				VFSFile file = (VFSFile)Target;
				m_Target = Resource.Load<Material>(file.FullPath);
			}
			else // Is Material object
				m_Target = Target as Material;
			if(m_Target != null) 
				m_Settings = (MaterialImportSettings)m_Target.ImportSettings;
		}

		public override void DrawInspector()
		{
			if (!m_Target)
				return;

			ImGUI.SetNextItemWidth(ImGUI.ContentRegionAvailable.x);
			ImGUI.Text(m_Target.ResourcePath, Colour.Grey);
			ImGUI.Separator();

			SetupTable();

			m_Settings.Shader = DrawResource<Shader>("Shader", m_Settings.Shader);
			m_Settings.Albedo = Draw("Colour", m_Settings.Albedo);
			m_Settings.AlbedoMap = DrawResource<Texture>("Albedo", m_Settings.AlbedoMap);
			m_Settings.AlphaClipping = Draw("Alpha Clip", m_Settings.AlphaClipping);
			m_Settings.AlphaThreshold = Draw("Alpha Threshold", m_Settings.AlphaThreshold, new RangeAttribute(0, 1));

			ImGUI.EndTable();

			if (!PendingChanges())
				return;

			Vector2 region = ImGUI.ContentRegionAvailable;
			if (ImGUI.Button("Revert", new Vector2(region.x * 0.5f, 0)))
				m_Settings = (MaterialImportSettings)m_Target.ImportSettings; // Re-import using previous settings
			ImGUI.SameLine();
			if (ImGUI.Button("Apply", new Vector2(region.x * 0.5f, 0)))
			{
				// Get latest settings. These are set in the object directly in DrawInspector(..) above
				m_Target.Import(m_Settings);

				// Save changes to disk
				Resource.SaveToDisk(m_Target);
			}
		}

		private bool PendingChanges() => m_Settings.Shader != (m_Target.Shader?.ResourceID ?? UUID.Invalid) ||
										 m_Settings.Albedo != m_Target.Albedo ||
										 m_Settings.AlbedoMap != (m_Target.AlbedoMap?.ResourceID ?? UUID.Invalid) ||
										 m_Settings.AlphaClipping != m_Target.AlphaClipping ||
										 m_Settings.AlphaThreshold != m_Target.AlphaThreshold;
	}
}