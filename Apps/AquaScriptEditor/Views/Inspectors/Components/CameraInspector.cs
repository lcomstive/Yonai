using AquaEngine;

namespace AquaEditor.Inspectors
{
	[CustomInspector(typeof(Camera))]
	public class CameraInspector : ComponentInspector
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

			if (ImGUI.Button("Set Main"))
				Camera.Main = m_Target;
		}
	}
}