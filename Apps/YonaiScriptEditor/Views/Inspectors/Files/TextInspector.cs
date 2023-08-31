using Yonai.IO;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(".txt")]
	public class TextInspector : CustomInspector
	{
		private string m_Text;

		public override void OnTargetChanged() => m_Text = VFS.ReadText(((VFSFile)Target).FullPath);

		public override void DrawInspector()
		{
			if(!string.IsNullOrEmpty(m_Text))
				ImGUI.Text(m_Text);
		}
	}
}