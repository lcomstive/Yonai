using AquaEngine;
using AquaEngine.IO;

namespace AquaEditor.Inspectors
{
	[CustomInspector(".mp3", ".wav")]
	public class SoundInspector : CustomInspector
	{
		private Sound m_Target;

		public override void OnTargetChanged() =>
			m_Target = Resource.Load<Sound>((VFSFile)Target);

		public override void DrawInspector()
		{
			ImGUI.SetNextItemWidth(ImGUI.ContentRegionAvailable.x);
			ImGUI.Text(m_Target.ResourcePath, Colour.Grey);
			ImGUI.Separator();
		}
	}
}