using System;
using AquaEngine;
using System.Linq;
using AquaEditor.Views;
using System.Reflection;
using AquaEditor.EditorUI;
using AquaEngine.Graphics;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace AquaEditor
{
	public class EditorUIService : AquaSystem
	{
		public static Texture MissingTexture { get; private set; }

		private MenuItemData m_RootMenuItem = new MenuItemData("Root");

		/// <summary>
		/// All currently open views/windows
		/// </summary>
		private static Dictionary<Type, View> m_ActiveViews = new Dictionary<Type, View>();

		protected override void Enabled()
		{
			SceneManager.WorldChanged += (world, added) =>
				Log.Info($"World '{world.Name}' [{world.ID}] has been " + (added ? "added" : "removed"));

			try
			{
				CompileMenuItems();
				InspectorView.GetCustomInspectors();

				TextureImportSettings textureSettings = new TextureImportSettings();
				textureSettings.FilePath = "assets://Textures/Test.png";
				m_TextureID = Resource.Load<Texture>("Textures/UI_Testing", textureSettings);

				textureSettings.FilePath = "assets://Textures/Black.jpg";
				UUID missingTextureID = Resource.Load<Texture>("Textures/Missing", textureSettings);
				MissingTexture = Resource.Get<Texture>(missingTextureID);

				// Demo //
				// GenerateConsoleLines();

				Open<SceneView>();
				Open<HierarchyView>();
				Open<InspectorView>();

				// CreateTestScene();
				LoadTestScene();
			}
			catch (Exception e) { Log.Exception(e); }
		}

		protected override void Disabled()
		{
			m_TestWorld?.Destroy();
			foreach (var view in m_ActiveViews.Values)
				view._Close();
		}

		protected override void Update()
		{
			foreach (var pair in m_ActiveViews)
				pair.Value._Update();
		}

		private const string SceneDir = "project://Assets/Scenes/";
		[MenuItem("File/Scene/Save", Shortcut = "CTRL+S")]
		private static void SaveScene()
		{
			if (!VFS.Exists(SceneDir))
				VFS.CreateDirectory(SceneDir);

			World[] activeScenes = SceneManager.GetActiveScenes();
			foreach (World scene in activeScenes)
				VFS.Write($"{SceneDir}{scene.Name}.json", JsonConvert.SerializeObject(scene.OnSerialize(), Formatting.Indented));
		}

		[MenuItem("File/Scene/Load")]
		private static void LoadScene()
		{
			World[] scenes = SceneManager.GetActiveScenes();
			SceneManager.UnloadAll();
			foreach (World scene in scenes)
			{
				// Check if exists
				if (!VFS.Exists($"{SceneDir}{scene.Name}.json"))
					continue;

				scene.OnDeserialize(JsonConvert.DeserializeObject<JObject>(VFS.ReadText($"{SceneDir}{scene.Name}.json")));
				SceneManager.Load(scene, SceneAddType.Additive);
			}
		}

		[MenuItem("File/Resources/Save")]
		private static void SaveResources() => Resource.SaveDatabase();

		[MenuItem("File/Resources/Load")]
		private static void LoadResources() => Resource.LoadDatabase();


		private static readonly Dictionary<ImGUI.StyleVar, float> StyleVarFloats = new Dictionary<ImGUI.StyleVar, float>()
		{
			{ ImGUI.StyleVar.FrameRounding, 2 },
			{ ImGUI.StyleVar.ChildRounding, 2 },
			{ ImGUI.StyleVar.ScrollbarRounding, 2 },
			{ ImGUI.StyleVar.ScrollbarSize, 15 }
		};

		private static readonly Dictionary<ImGUI.StyleVar, Vector2> StyleVarVectors = new Dictionary<ImGUI.StyleVar, Vector2>()
		{
			{ ImGUI.StyleVar.WindowPadding, new Vector2(7.5f, 5.0f) },
		};

		private static readonly Dictionary<ImGUI.StyleColour, Colour> StyleColours = new Dictionary<ImGUI.StyleColour, Colour>()
		{
			{ ImGUI.StyleColour.FrameBg, Colour.FromHex("#3b3f43") },
			{ ImGUI.StyleColour.FrameBgHovered, Colour.FromHex("#49535c") },
			{ ImGUI.StyleColour.FrameBgActive, Colour.FromHex("#606c78") },

			{ ImGUI.StyleColour.MenuBarBg, Colour.FromHex("#212428") },
			{ ImGUI.StyleColour.DockingEmptyBg, Colour.FromHex("#212428") },

			{ ImGUI.StyleColour.Separator, Colour.FromHex("#212428") },
			{ ImGUI.StyleColour.SeparatorHovered, Colour.FromHex("#3D454F") },
			{ ImGUI.StyleColour.SeparatorActive, Colour.FromHex("#5E6B7C") },

			{ ImGUI.StyleColour.Border, Colour.FromHex("#212428") },
			{ ImGUI.StyleColour.Header, new Colour(0, 0, 0, 0) },
			{ ImGUI.StyleColour.HeaderHovered, new Colour(1, 1, 1, 0.1f) },
			{ ImGUI.StyleColour.HeaderActive, new Colour(1, 1, 1, 0.15f) },
			{ ImGUI.StyleColour.TextSelectedBg, new Colour(1, 1, 1, 0.2f) },
		};

		protected override void Draw()
		{
			try
			{
				foreach (var pair in StyleVarFloats) ImGUI.PushStyleVar(pair.Key, pair.Value);
				foreach (var pair in StyleVarVectors) ImGUI.PushStyleVar(pair.Key, pair.Value);
				foreach (var style in StyleColours) ImGUI.PushStyleColour(style.Key, style.Value);

				BeginDockspace();
				DrawMenuBar();

				ImGUI.Begin("Demo");
				DrawDemoContents();
				ImGUI.End();

				View[] views = m_ActiveViews.Values.ToArray();
				for (int i = 0; i < views.Length; i++)
				{
					try { views[i]._Draw(); }
					catch (Exception e)
					{
						Log.Exception(e);
						// Remove view with exception, otherwise it will (likely) do it each frame
						m_ActiveViews.Remove(m_ActiveViews.Keys.ElementAt(i));
					}
				}

				EndDockspace();

				ImGUI.PopStyleColour(StyleColours.Count);
				ImGUI.PopStyleVar(StyleVarFloats.Count + StyleVarVectors.Count);
			}
			catch (Exception e) { Log.Exception(e); }
		}

		#region View Handling
		public static void Open<T>() where T : View, new()
		{
			Type type = typeof(T);

			// Check if window of type is already open
			if (!m_ActiveViews.ContainsKey(type))
			{
				T instance = new T();
				try
				{
					// Inform of opening
					instance._Open();
					m_ActiveViews.Add(type, instance);
				}
				catch (Exception e) { Log.Exception(e); }
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

			ImGUI.PushStyleVar(ImGUI.StyleVar.ItemSpacing, new Vector2(10, 7.5f));

			foreach (MenuItemData menuItem in m_RootMenuItem.Children)
				menuItem.Render();

			ImGUI.PopStyleVar();
			ImGUI.EndMenuBar();
		}

		private void CompileMenuItems()
		{
			// Add default menu item directories
			m_RootMenuItem.AddDirectory("File");
			m_RootMenuItem.AddDirectory("Window");

			BindingFlags bindingFlags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static;

			// Get all methods with the MenuItem attribute, in all assemblies
			try
			{
				List<(MethodInfo, MenuItemAttribute)> menuItems = new List<(MethodInfo, MenuItemAttribute)>();
				Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
				foreach (Assembly assembly in assemblies)
				{
					try
					{
						foreach (Type type in assembly.GetTypes())
						{
							foreach (MethodInfo method in type.GetMethods(bindingFlags))
							{
								MenuItemAttribute attribute = method.GetCustomAttribute<MenuItemAttribute>();
								if (attribute != null)
									menuItems.Add((method, attribute));
							}
						}
					}
					catch { }
				}

				foreach ((MethodInfo method, MenuItemAttribute attribute) in menuItems)
					m_RootMenuItem.Add(method, attribute);
			}
			catch (Exception e) { Log.Exception(e); }
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

		private UUID m_TextureID = UUID.Invalid;
		private string m_Input = "String input field";
		private string m_Password = "Password";
		private string m_MultilineInput = "Multiline\nString\nField";

		private float[] m_FPSValues = new float[100];

		private World m_TestWorld = null;
		private const string TestSceneName = "Test World";

		private void CreateTestScene()
		{
			m_TestWorld = World.Create(TestSceneName);
			SceneManager.Load(m_TestWorld, SceneAddType.Additive);

			for (int i = 0; i < 15; i++)
			{
				Entity e = m_TestWorld.CreateEntity();
				NameComponent nameComponent = e.AddComponent<NameComponent>();
				nameComponent.Name = "Entity";

				SpriteRenderer renderer = e.AddComponent<SpriteRenderer>();
				renderer.Sprite = Resource.Load<Texture>(
					"Textures/Texture/Test_Texture09",
					new TextureImportSettings("assets://Textures/texture_09.png")
				);
				renderer.Shader = Resource.Load<Shader>("Shaders/NewSpriteShader", new ShaderImportSettings()
				{
					VertexPath = "assets://Shaders/Sprite.vert",
					FragmentPath = "assets://Shaders/NewSprite.frag"
				});

				e.AddComponent<Transform>().Position = Vector3.Right * i * 2.5f;
			}
		}

		private void LoadTestScene()
		{
			Resource.Load<Texture>(
				"Textures/Texture/Test_Texture09",
				new TextureImportSettings("assets://Textures/texture_09.png")
			);
			Resource.Load<Shader>("Shaders/NewSpriteShader", new ShaderImportSettings()
			{
				VertexPath = "assets://Shaders/Sprite.vert",
				FragmentPath = "assets://Shaders/NewSprite.frag"
			});

			if (!VFS.Exists($"{SceneDir}{TestSceneName}.json"))
				return; // Not found

			m_TestWorld = World.Create(JsonConvert.DeserializeObject<JObject>(VFS.ReadText($"{SceneDir}{TestSceneName}.json")));
			SceneManager.Load(m_TestWorld, SceneAddType.Single);
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
			if (m_ConsoleLines != null)
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

		private List<(string, Colour)> m_ConsoleLines;
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

			m_ConsoleLines = new List<(string, Colour)>();
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