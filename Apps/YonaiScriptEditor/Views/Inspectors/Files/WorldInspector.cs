using Yonai;
using Yonai.IO;
using YonaiEditor.Views;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(World), ".world")]
	public class WorldInspector : CustomInspector
	{
		private World m_Target;

		public override void OnTargetChanged()
		{
			// Check if filepath is selected (resource view)
			if (Target is VFSFile)
			{
				VFSFile file = (VFSFile)Target;
				m_Target = Resource.Load<World>(file.FullPath);

				// Load scene from file
				SceneManager.Load(m_Target);
				InspectorView.Target = null;
			}
			else // Is world object, wanting to inspect
				m_Target = Target as World;
		}

		public override void DrawInspector()
		{
			ImGUI.Text(m_Target.ResourcePath, Colour.Grey);
			ImGUI.Text($"Entities: {m_Target.EntityCount}", Colour.Grey);
			
			ImGUI.BeginChild("WorldInspectorSystems");
			{
				YonaiSystem[] systems = m_Target.GetSystems();
				ImGUI.Text($"Systems: {systems.Length}", Colour.Grey);
				ImGUI.Space();

				foreach (YonaiSystem system in systems)
				{
					ImGUI.Separator();
					if (!ImGUI.Foldout(system.GetType().Name, true))
						continue;

					SetupTable();
					DrawInspector(system);
					ImGUI.EndTable();
				}
			}
			ImGUI.EndChild();
		}
	}
}