using System;
using Yonai;
using Yonai.IO;
using System.Linq;
using System.Reflection;
using YonaiEditor.Views;
using System.Collections.Generic;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(World), ".world")]
	public class WorldInspector : CustomInspector
	{
		private World m_Target;

		private static List<Type> s_SystemTypes = new List<Type>();

		public override void Opened()
		{
			if (s_SystemTypes.Count == 0)
				CacheSystems();
		}

		public override void OnTargetChanged()
		{
			// Check if filepath is selected (resource view)
			if (Target is VFSFile)
			{
				VFSFile file = (VFSFile)Target;
				m_Target = Resource.Load<World>(file.FullPath);

				// Load scene from file
				SceneManager.Load(m_Target, SceneAddType.Single, false);
				InspectorView.Target = null;
			}
			else // Is world object, wanting to inspect
				m_Target = Target as World;
		}

		public override void DrawInspector()
		{
			ImGUI.Text(m_Target.ResourcePath, Colour.Grey);
			ImGUI.Text($"Entities: {m_Target.EntityCount}", Colour.Grey);
			
			ImGUI.BeginChild("WorldInspectorSystems");
			{
				YonaiSystem[] systems = m_Target.GetSystems();
				ImGUI.Text($"Systems: {systems.Length}", Colour.Grey);
				ImGUI.Space();

				float availableWidth = ImGUI.ContentRegionAvailable.x;

				for(int i = systems.Length - 1; i >= 0; i--)
				{
					YonaiSystem system = systems[i];

					if (!ImGUI.Foldout(system.GetType().Name, true))
						continue;

					SetupTable();
					DrawInspector(system);
					ImGUI.EndTable();

					Vector2 buttonSize = new Vector2(15, 15);
					ImGUI.SetCursorPosX(availableWidth - buttonSize.x * 4);
					if (ImGUI.ButtonImage(Icons.Get("Return"), buttonSize))
					{
						m_Target.RemoveSystem(system.GetType());
						m_Target.AddSystem(system.GetType());
					}
					if (ImGUI.IsItemHovered()) ImGUI.SetTooltip("Reset to default values");

					ImGUI.SameLine();
					if (ImGUI.ButtonImage(Icons.Get("Cross"), buttonSize))
						m_Target.RemoveSystem(system.GetType());
					if (ImGUI.IsItemHovered()) ImGUI.SetTooltip("Remove system");
					
					ImGUI.Space();
				}

				ImGUI.Space();

				if (ImGUI.Button("Add System", new Vector2(availableWidth, 0)))
					AddSystemPrompt();
			}
			ImGUI.EndChild();
		}
		
		private void AddSystemPrompt()
		{
			// Get all systems available
			Type[] types = s_SystemTypes.Where(x => !m_Target.HasSystem(x)).ToArray();

			string[] typeNames = new string[types.Length];
			for (int i = 0; i < types.Length; i++)
				typeNames[i] = $"{types[i].FullName}, {types[i].Assembly.GetName().Name}";

			// Show search panel
			SearchView.Search(typeNames, (typeString) =>
			{
				if (string.IsNullOrEmpty(typeString))
					return;
				YonaiSystem system = m_Target.AddSystem(Type.GetType(typeString));
				system.Enable(EditorService.State != EditorState.Edit);
			});
		}

		private static void CacheSystems()
		{
			Log.Debug("Caching Yonai system types for world inspector");
			Type systemType = typeof(YonaiSystem);
			Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
			foreach (Assembly assembly in assemblies)
			{
				try
				{
					// Get all types in assembly that derive from Component
					foreach (Type type in assembly.GetTypes().Where(x => x.IsSubclassOf(systemType) && x.IsClass && !x.IsAbstract))
					{
						try
						{
							bool isGlobal = (bool)type.GetProperty("IsGlobal", typeof(bool))
												.GetValue(Activator.CreateInstance(type));
							bool editorService = type.Namespace.StartsWith("YonaiEditor");
							if (!isGlobal && !editorService)
							{
								Log.Debug(" - " + type.FullName);
								s_SystemTypes.Add(type);
							}
						}
						catch(Exception e) { Log.Exception(e, type.FullName); }
					}
				}
				catch { }
			}
			Log.Debug($"Found {s_SystemTypes.Count} Yonai system types");
		}

	}
}