using Yonai;
using Yonai.IO;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(".mp3", ".wav", ".ogg")]
	public class AudioDataInspector : CustomInspector
	{
		private AudioData m_Target;

		public override void OnTargetChanged() =>
			m_Target = Resource.Load<AudioData>((VFSFile)Target);

		public override void DrawInspector()
		{
			ImGUI.SetNextItemWidth(ImGUI.ContentRegionAvailable.x);
			ImGUI.Text(m_Target.ResourcePath, Colour.Grey);
			ImGUI.Separator();
		}
	}
}