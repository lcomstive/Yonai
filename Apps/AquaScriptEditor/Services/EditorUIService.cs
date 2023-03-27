using AquaEngine;
using AquaEngine.Graphics;
using System;

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

		private float m_AngleRads = MathUtils.Deg2Rad(50);
		private IVector2 m_TestIVec2 = new IVector2(420, 69);

		private uint m_TextureID = uint.MaxValue;
		private string m_Input = "String input field";
		private string m_Password = "Password";
		private string m_MultilineInput = "Multiline\nString\nField";

		private float[] m_FPSValues = new float[100];

		protected override void Enabled() => m_TextureID = Resource.Load<Texture>("Textures/UI_Testing", "assets://Textures/Test.png");

		protected override void Draw()
		{
			ImGUI.Begin("Editor Test");
			DrawContents();
			ImGUI.End();
		}

		private void DrawContents()
		{
			ImGUI.Drag("Test Float", ref m_TestFloat, 0.1f, 0, 100, "%.2f%%");
			ImGUI.Drag("Test Vector2", ref m_TestVector2);
			ImGUI.Drag("Test Vector3", ref m_TestVector3);
			ImGUI.Drag("Test IVec2", ref m_TestIVec2, 10, 0, 1000, "%i");

			if (ImGUI.Button("Click Me"))
				Log.Debug("Button clicked");
			ImGUI.SameLine();
			ImGUI.Checkbox("Checkbox", ref m_Checkbox);

			ImGUI.Space();

			ImGUI.ColourEdit3("A", ref m_ColourA);
			ImGUI.ColourEdit3("B", ref m_ColourB);

			ImGUI.Slider(
				"Lerp Amount",
				ref m_LerpAmount,
				0.0f,       /* Min   */
				100.0f,     /* Max   */
				"%.0f%%"    /* Format */
				);

			ImGUI.BeginDisabled();
			Colour lerp = Colour.Lerp(m_ColourA, m_ColourB, m_LerpAmount / 100.0f);
			ImGUI.ColourEdit4("Combined", ref lerp);
			ImGUI.EndDisabled();

			ImGUI.Space();

			ImGUI.Slider("Slider Float", ref m_TestFloat, 0, 100, "%.2f%%");
			ImGUI.Slider("Slider IVec2", ref m_TestIVec2, 0, 1000, "%i");

			ImGUI.SliderAngle("Slider Angle", ref m_AngleRads);

			Texture texture = Resource.Get<Texture>(m_TextureID);
			ImGUI.Image(texture, new Vector2(100, 100), lerp, m_ColourA);

			ImGUI.SameLine();

			if (ImGUI.ButtonImage(m_TextureID, new Vector2(100, 100)))
				Log.Debug("Image button pressed");

			ImGUI.Space();

			ImGUI.Input("Input String", ref m_Input);
			ImGUI.InputPassword("Password", ref m_Password);
			ImGUI.InputTextMultiline("Multiline", ref m_MultilineInput);

			ImGUI.Input("Input Float", ref m_TestFloat);
			ImGUI.Input("Input Vector3", ref m_TestVector3);

			ImGUI.Text("Hover me for tooltip!");
			if (ImGUI.IsItemHovered())
			{
				if (ImGUI.BeginTooltip())
				{
					DrawContents();
					ImGUI.EndTooltip();
				}
			}

			UpdateFPSValues();
			ImGUI.PlotLines("FPS", m_FPSValues, $"FPS: {Time.FPS}");
		}

		// When reaches a threshold, adds an FPS value to m_FPSValues
		private float m_UpdateCounter = 0.0f;
		private const float UpdateCounterThreshold = .01f;
		private void UpdateFPSValues()
		{
			m_UpdateCounter += Time.DeltaTime;
			if (m_UpdateCounter >= UpdateCounterThreshold)
				m_UpdateCounter -= m_UpdateCounter;
			else
				return;

			// Shift all values down one
			for (int i = 0; i < m_FPSValues.Length - 1; i++)
				m_FPSValues[i] = m_FPSValues[i + 1];
			m_FPSValues[m_FPSValues.Length - 1] = Time.FPS;
		}
	}
}