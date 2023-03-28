using System;
using AquaEngine;
using System.Linq;
using AquaEngine.Graphics;
using System.Collections.Generic;
using AquaEngine.Systems;

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

		protected override void Start() => GenerateConsoleLines();

		protected override void Draw()
		{
			BeginDockspace();
			DrawMenuBar();

			ImGUI.Begin("Demo");
			DrawDemoContents();
			ImGUI.End();

			EndDockspace();
		}

		private void BeginDockspace()
		{
			// Create dockspace
			ImGUI.Viewport viewport = ImGUI.GetMainViewport();
			ImGUI.SetNextWindowPos(viewport.Position);
			ImGUI.SetNextWindowSize(viewport.Size);
			ImGUI.SetNextWindowViewport(viewport.ID);
			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowRounding, 0.0f);
			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowBorderSize, 0.0f);

			ImGUI.WindowFlags windowFlags = ImGUI.WindowFlags.MenuBar |
											ImGUI.WindowFlags.NoDocking | // Don't dock the docking space
											ImGUI.WindowFlags.NoTitleBar |
											ImGUI.WindowFlags.NoCollapse |
											ImGUI.WindowFlags.NoResize |
											ImGUI.WindowFlags.NoMove |
											ImGUI.WindowFlags.NoBringToFrontOnFocus |
											ImGUI.WindowFlags.NoNavFocus;

			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowPadding, Vector2.Zero);
			ImGUI.Begin("DockSpaceWindow", windowFlags);
			ImGUI.PopStyleVar(3);

			ImGUI.DockSpace("DockSpace");
		}

		private void EndDockspace() => ImGUI.End();

		private void DrawMenuBar()
		{
			if (!ImGUI.BeginMenuBar())
				return;

			if(ImGUI.BeginMenu("File"))
			{
				if (ImGUI.MenuItem("Exit"))
					Application.Exit();

				ImGUI.EndMenu();
			}

			if (ImGUI.MenuItem("Fullscreen"))
				Window.Fullscreen = Window.Fullscreen == FullscreenMode.Windowed ? FullscreenMode.Borderless : FullscreenMode.Windowed;

			ImGUI.EndMenuBar();
		}

		private void DrawDemoContents()
		{
			ImGUI.Input("Input String", ref m_Input);
			ImGUI.InputPassword("Password", ref m_Password);
			ImGUI.InputTextMultiline("Multiline", ref m_MultilineInput);

			ImGUI.Input("Input Float", ref m_TestFloat);
			ImGUI.Input("Input Vector3", ref m_TestVector3);
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

			ImGUI.Text("Hover me for tooltip!");
			if (ImGUI.IsItemHovered())
			{
				if (ImGUI.BeginTooltip())
				{
					DrawDemoContents();
					ImGUI.EndTooltip();
				}
			}

			UpdateFPSValues();
			ImGUI.PlotLines("FPS", m_FPSValues, $"FPS: {Time.FPS}");
			
			ImGUI.BeginChild("Console", new Vector2(0, 150), true);
			foreach ((string msg, Colour colour) in m_ConsoleLines)
				ImGUI.Text(msg, colour);
			ImGUI.EndChild();
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

		private List<(string, Colour)> m_ConsoleLines = new List<(string, Colour)>();
		private const int ConsoleLinesGenerated = 20;
		private void GenerateConsoleLines()
		{
			string[] consoleLines =
{
				"Test",
				"Another one",
				"A line",
				"Testing"
			};
			List<Colour> consoleColours = new List<Colour>()
			{
				Colour.Black,
				Colour.White,
				Colour.Grey,
				Colour.Red,
				Colour.Green,
				Colour.Blue,
				Colour.Yellow,
				Colour.Magenta,
				Colour.Purple,
				Colour.Orange,
				Colour.Turqoise
			};

			for (int i = 0; i < ConsoleLinesGenerated; i++)
			{
				m_ConsoleLines.Add((
					AquaEngine.Random.Range(consoleLines),
					AquaEngine.Random.Range(consoleColours)
				));
			}
		}
	}
}