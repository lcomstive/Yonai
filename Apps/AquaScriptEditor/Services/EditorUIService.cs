using AquaEngine;

namespace AquaEditor
{
	public class EditorUIService : AquaSystem
	{
		private float m_TestFloat = 69.69f;
		private Vector2 m_TestVector2 = new Vector2(6, 9);
		private Vector3 m_TestVector3 = new Vector3(4, 2, 0);
		private bool m_Checkbox = true;

		private Colour m_ColourA = Colour.Blue;
		private Colour m_ColourB = Colour.Green;
		private float m_LerpAmount = 50.0f;

		private IVector2 m_TestIVec2 = new IVector2(420, 69);

		private float m_AngleRads = MathUtils.Deg2Rad(50);

		protected override void Draw()
		{
			ImGUI.Begin("Editor Test");
			{
				ImGUI.Drag("Test Float", ref m_TestFloat, 0.1f, 0, 100, "%.2f%%");
				ImGUI.Drag("Test Vector2", ref m_TestVector2);
				ImGUI.Drag("Test Vector3", ref m_TestVector3);
				ImGUI.Drag("Test IVec2", ref m_TestIVec2, 10, 0, 1000, "%i");

				if (ImGUI.Button("Click Me"))
					Log.Debug("Button clicked");

				ImGUI.Checkbox("Checkbox", ref m_Checkbox);

				ImGUI.Space();

				ImGUI.ColourEdit3("A", ref m_ColourA);
				ImGUI.ColourEdit3("B", ref m_ColourB);

				ImGUI.Slider(
					"Lerp Amount",
					ref m_LerpAmount,
					0.0f,		/* Min   */
					100.0f,		/* Max   */
					"%.0f%%"	/* Format */
					);

				Colour lerp = Colour.Lerp(m_ColourA, m_ColourB, m_LerpAmount / 100.0f);
				ImGUI.ColourEdit4("Combined", ref lerp);

				ImGUI.Space();

				ImGUI.Slider("Slider Float", ref m_TestFloat, 0, 100, "%.2f%%");
				ImGUI.Slider("Slider IVec2", ref m_TestIVec2, 0, 1000, "%i");

				ImGUI.SliderAngle("Slider Angle", ref m_AngleRads);
			}
			ImGUI.End();
		}
	}
}