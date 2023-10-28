using Yonai;
using System;
using Yonai.IO;
using System.Linq;
using Yonai.Graphics;
using Newtonsoft.Json;
using YonaiEditor.Views;
using System.Reflection;
using YonaiEditor.EditorUI;
using Newtonsoft.Json.Linq;
using YonaiEditor.Commands;
using YonaiEditor.BuildProcess;
using System.Collections.Generic;

namespace YonaiEditor.Systems
{
	public class EditorUIService : YonaiSystem
	{
		public static Texture MissingTexture { get; private set; }

		private MenuItemData m_RootMenuItem = new MenuItemData("Root");

		/// <summary>
		/// All currently open views/windows
		/// </summary>
		private static Dictionary<Type, View> m_ActiveViews = new Dictionary<Type, View>();

		private EditorState State
		{
			get => EditorService.State;
			set => EditorService.State = value;
		}

		protected override void Start()
		{
			SceneManager.WorldChanged += (world, added) =>
				Log.Info($"World '{world.Name}' [{world.ID}] has been " + (added ? "added" : "removed"));

			try
			{
				Icons.Load();
				CompileMenuItems();
				InspectorView.GetCustomInspectors();
				SetupShortcuts();

				Open<GameView>();
				Open<SceneView>();
				Open<ConsoleView>();
				Open<HierarchyView>();
				Open<InspectorView>();
				Open<ResourcesView>();

				World world = Resource.Load<World>("project://Assets/Scenes/Test World.json");
				SceneManager.Load(world, SceneAddType.Single, false);

				if (!Scripting.IsAssemblyReloading())
				{
					if (Window.Resolution.x < 1600 || Window.Resolution.y < 900)
						Window.Resolution = new IVector2(1600, 900);
					EditorWindow.Show();
				}
			}
			catch (Exception e) { Log.Exception(e); }
		}

		protected override void Destroyed()
		{
			foreach (var view in m_ActiveViews.Values)
				view._Close();
		}

		protected override void Update()
		{
			View[] views = m_ActiveViews.Values.ToArray();
			foreach (View view in views)
			{
				try { view._Update(); }
				catch (Exception e)
				{
					Log.Exception(e, $"View '{view.GetType().Name}' failed");
					m_ActiveViews.Remove(view.GetType());
				}
			}

			try { ShortcutManager.Update(); }
			catch(Exception e) { Log.Exception(e); }
		}

		private void SetupShortcuts()
		{
			ShortcutManager.Map(Application.Exit, Key.LeftControl, Key.Q);
			ShortcutManager.Map(CommandHistory.Redo, Key.LeftControl, Key.LeftShift, Key.Z);
			ShortcutManager.Map(CommandHistory.Undo, Key.LeftControl, Key.Z);
			ShortcutManager.Map(CommandHistory.Redo, Key.LeftControl, Key.Y);
		}

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
		public static T Open<T>() where T : View, new()
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
					return instance;
				}
				catch (Exception e) { Log.Exception(e); }
			}
			return null;
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

		public static T GetView<T>() where T : View
		{
			Type type = typeof(T);
			return m_ActiveViews.ContainsKey(type) ? (T)m_ActiveViews[type] : null;
		}
		#endregion

		internal static void BeginDockspace()
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

			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowPadding, new Vector2(10, 10));
			ImGUI.PushStyleVar(ImGUI.StyleVar.ItemSpacing, new Vector2(10, 7.5f));
			ImGUI.PushStyleColour(ImGUI.StyleColour.Button, Colour.None);

			foreach (MenuItemData menuItem in m_RootMenuItem.Children)
				menuItem.Render();

			DrawPlayButton();
			DrawVersionString();

			ImGUI.PopStyleVar(2);
			ImGUI.PopStyleColour();
			ImGUI.EndMenuBar();
		}

		private void DrawPlayButton()
		{
			EditorState state = State;
			Vector2 iconSize = new Vector2(12.5f, 12.5f);

			ImGUI.SetCursorPosX(ImGUI.WindowSize.x / 2.0f - (iconSize.x + 2.5f) * (state == EditorState.Edit ? 1 : 3));

			string icon = "Right";
			if (state == EditorState.Play)
				icon = "Pause";

			if (state != EditorState.Edit && ImGUI.ButtonImage(Icons.Get("Stop"), iconSize))
				State = EditorState.Edit;
			if (ImGUI.ButtonImage(Icons.Get(icon), iconSize))
				State = State == EditorState.Play ? EditorState.Pause : EditorState.Play;
			if (state != EditorState.Edit && ImGUI.ButtonImage(Icons.Get("Next"), iconSize))
				State = EditorState.Step;
		}

		private void DrawVersionString()
		{
			string versionString = "v" + Application.EngineVersion.ToLongString();

			Vector2 textSize = ImGUI.CalculateTextWidth(versionString);
			ImGUI.SetCursorPosX(ImGUI.WindowSize.x - textSize.x - 10);
			ImGUI.Text(versionString, Colour.Grey);
		}

		#region Menu Items
		private void CompileMenuItems()
		{
			// Add default menu item directories
			m_RootMenuItem.AddDirectory("File");
			m_RootMenuItem.AddDirectory("File/Scene");
			m_RootMenuItem.AddDirectory("File/Resources");
			m_RootMenuItem.AddDirectory("Window");
			m_RootMenuItem.AddDirectory("Build");

			GameBuilder.ConstructBuildMenu();

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

		public void AddMenuItem(Action callback, MenuItemAttribute attribute) => m_RootMenuItem.Add(callback, attribute);
		public void AddMenuItemDirectory(string directory) => m_RootMenuItem.AddDirectory(directory);
		#endregion
	}
}