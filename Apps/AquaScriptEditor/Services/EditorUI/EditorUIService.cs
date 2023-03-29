using System;
using AquaEngine;
using System.Linq;
using AquaEditor.Views;
using System.Reflection;
using AquaEditor.EditorUI;
using AquaEngine.Graphics;
using System.Collections.Generic;

namespace AquaEditor
{
	public class EditorUIService : AquaSystem
	{
		private MenuItemData m_RootMenuItem = new MenuItemData("Root");

		/// <summary>
		/// All currently open views/windows
		/// </summary>
		private static Dictionary<Type, View> m_ActiveViews = new Dictionary<Type, View>();

		protected override void Start()
		{
			CompileMenuItems();

			m_TextureID = Resource.Load<Texture>("Textures/UI_Testing", "assets://Textures/Test.png");

			World.Get(0).GetEntity(0).AddComponent<NameComponent>().Name = "Camera";

			// Demo //
			GenerateConsoleLines();
		}

		protected override void Update()
		{
			foreach (var pair in m_ActiveViews)
				pair.Value._Update();
		}

		protected override void Draw()
		{
			BeginDockspace();
			DrawMenuBar();

			ImGUI.Begin("Demo");
			DrawDemoContents();
			ImGUI.End();

			View[] views = m_ActiveViews.Values.ToArray();
			foreach (View view in views)
				view._Draw();

			EndDockspace();
		}

		#region View Handling
		public static void Open<T>() where T : View, new()
		{
			Type type = typeof(T);

			// Check if window of type is already open
			if (!m_ActiveViews.ContainsKey(type))
			{
				T instance = new T();
				instance._Open(); // Inform of opening
				m_ActiveViews.Add(type, instance);
			}
		}

		public static void Close<T>() where T : View
		{
			Type type = typeof(T);
			// Check that window of type is already open
			if (m_ActiveViews.ContainsKey(type))
			{
				m_ActiveViews[type]._Close(); // Inform of closure
				m_ActiveViews.Remove(type);
			}
		}
		#endregion

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

			foreach (MenuItemData menuItem in m_RootMenuItem.Children)
				menuItem.Render();

			ImGUI.EndMenuBar();
		}

		private void CompileMenuItems()
		{
			// Add default menu item directories
			m_RootMenuItem.AddDirectory("File");
			m_RootMenuItem.AddDirectory("Window");

			BindingFlags bindingFlags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static;
			
			// Get all methods with the MenuItem attribute, in all assemblies
			var menuItems =
				from assembly in AppDomain.CurrentDomain.GetAssemblies()
				from type in assembly.GetTypes()
				from method in type.GetMethods(bindingFlags)
				let attribute = method.GetCustomAttribute<MenuItemAttribute>()
				where attribute != null
				select new
				{
					Method = method,
					Attribute = attribute
				};


			foreach(var menuItem in menuItems)
				m_RootMenuItem.Add(menuItem.Method, menuItem.Attribute);					
		}

		#region Demo
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
		#endregion
	}
}