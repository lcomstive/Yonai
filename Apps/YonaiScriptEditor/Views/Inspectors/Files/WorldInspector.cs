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
			}
			else // Is world object
				m_Target = Target as World;

			SceneManager.Load(m_Target);
			InspectorView.Target = null;
		}
	}
}