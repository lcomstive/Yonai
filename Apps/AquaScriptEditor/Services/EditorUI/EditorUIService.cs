using System;
using AquaEngine;
using System.Linq;
using Newtonsoft.Json;
using AquaEditor.Views;
using System.Reflection;
using AquaEditor.EditorUI;
using AquaEngine.Graphics;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;

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

				Open<SceneView>();
				Open<HierarchyView>();
				Open<InspectorView>();
				Open<ResourcesView>();

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
				try
				{
					// Check if exists
					if (!VFS.Exists($"{SceneDir}{scene.Name}.json"))
						continue;

					scene.OnDeserialize(JsonConvert.DeserializeObject<JObject>(VFS.ReadText($"{SceneDir}{scene.Name}.json")));
					SceneManager.Load(scene, SceneAddType.Additive);
				}
				catch(Exception e) { Log.Exception(e); }
			}
		}

		[MenuItem("File/Scene/Recreate Test Scene", PrependSeparator = true)]
		private static void RecreateTestScene()
		{
			EditorUIService uiService = Get<EditorUIService>();
			if(uiService.m_TestWorld)
			{
				SceneManager.Unload(uiService.m_TestWorld);
				uiService.m_TestWorld.Destroy();
				uiService.m_TestWorld = null;
			}
			uiService.CreateTestScene();
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

		#region Test Scene
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
			SceneManager.Load(m_TestWorld, SceneAddType.Additive);
		}
		#endregion
	}
}