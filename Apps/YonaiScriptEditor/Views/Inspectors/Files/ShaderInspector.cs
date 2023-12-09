using Yonai;
using Yonai.IO;
using Yonai.Graphics;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(Shader), ".shader")]
	public class ShaderInspector : CustomInspector
	{
		private Shader m_Target;
		private ShaderImportSettings m_Settings;

		public override void OnTargetChanged()
		{
			// Check if filepath is selected (resource view)
			if (Target is VFSFile)
			{
				VFSFile file = (VFSFile)Target;
				m_Target = Resource.Load<Shader>(file.FullPath, new ShaderImportSettings());
			}
			else // Is Shader object
				m_Target = Target as Shader;
			m_Settings = (ShaderImportSettings)m_Target.ImportSettings;
		}

		public override void DrawInspector()
		{
			if (!m_Target)
				return;

			ImGUI.SetNextItemWidth(ImGUI.ContentRegionAvailable.x);
			ImGUI.Text(m_Target.ResourcePath, Colour.Grey);
			ImGUI.Separator();

			SetupTable();

			/*
			if (DrawFilepath("Vertex", m_Settings.VertexPath, out VFSFile vertexPath, ".vert", ".vertex"))
				m_Settings.VertexPath = vertexPath;
			if (DrawFilepath("Fragment", m_Settings.FragmentPath, out VFSFile fragmentPath, ".frag", ".fragment"))
				m_Settings.FragmentPath = fragmentPath;
			if (DrawFilepath("Compute", m_Settings.ComputePath, out VFSFile computePath, ".compute"))
				m_Settings.ComputePath = computePath;
			if (DrawFilepath("Geometry", m_Settings.GeometryPath, out VFSFile geometryPath, ".geo", ".geometry"))
				m_Settings.GeometryPath = geometryPath;
			*/

			ImGUI.EndTable();

			if (!PendingChanges())
				return;

			Vector2 region = ImGUI.ContentRegionAvailable;
			if (ImGUI.Button("Revert", new Vector2(region.x * 0.5f, 0)))
				m_Settings = (ShaderImportSettings)m_Target.ImportSettings; // Re-import using previous settings
			ImGUI.SameLine();
			if (ImGUI.Button("Apply", new Vector2(region.x * 0.5f, 0)))
			{
				// Get latest settings. These are set in the object directly in DrawInspector(..) above
				m_Target.Import(m_Settings);

				// Save changes to disk
				Resource.SaveToDisk(m_Target);
			}
		}


		private bool PendingChanges() => false;
		/*
			m_Settings.VertexPath != m_Target.ShaderStages.VertexPath ||
											m_Settings.FragmentPath != m_Target.ShaderStages.FragmentPath ||
											m_Settings.ComputePath != m_Target.ShaderStages.ComputePath ||
											m_Settings.GeometryPath != m_Target.ShaderStages.GeometryPath;
		*/
	}
}