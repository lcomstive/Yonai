using Yonai;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(Camera))]
	public class CameraInspector : CustomInspector
	{
		private Camera m_Target;

		public override void OnTargetChanged()
		{
			base.OnTargetChanged();
			m_Target = (Camera)Target;
		}

		public override void DrawInspector()
		{
			base.DrawInspector();
			if (ImGUI.Button("Set Main", new Vector2(ImGUI.ContentRegionAvailable.x, 0)))
				Camera.Main = m_Target;
		}
	}
}