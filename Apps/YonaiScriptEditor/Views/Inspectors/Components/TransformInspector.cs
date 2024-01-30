using Yonai;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(Transform))]
	public class TransformInspector : CustomInspector
	{
		private Transform m_Target;

		public override void OnTargetChanged()
		{
			base.OnTargetChanged();
			m_Target = (Transform)Target;
		}

		public override bool OnReset()
		{
			m_Target.LocalPosition = Vector3.Zero;
			m_Target.LocalRotation = Quaternion.Identity;
			m_Target.LocalScale = Vector3.One;

			return true;
		}
	}
}