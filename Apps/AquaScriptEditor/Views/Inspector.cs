using AquaEngine;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;

namespace AquaEditor.Views
{
	public class InspectorView : View
	{
		private static object m_Target = null;
		private static CustomInspector s_CurrentInspector = null;

		/// <summary>
		/// Target object to show properties for
		/// </summary>
		public static object Target
		{
			get => m_Target;
			set
			{
				if (m_Target == value) return; // No change
				Type previousType = m_Target?.GetType();
				Type newType = value?.GetType();

				// Assign target
				m_Target = value;

				// Check for inspector type changes
				if (previousType == newType && newType != typeof(VFSFile))
				{
					if(s_CurrentInspector != null)
					{
						s_CurrentInspector.Target = m_Target;
						s_CurrentInspector.OnTargetChanged();
					}
					return; // Nothing further needs to be done
				}

				// Inform existing inspector of closure
				s_CurrentInspector?.Closed();

				if (newType == typeof(VFSFile))
				{
					VFSFile vfsFile = (VFSFile)value;
					s_CurrentInspector = FindInspector(vfsFile);
				}
				else
					s_CurrentInspector = FindInspector(newType);

				if(s_CurrentInspector != null)
				{
					s_CurrentInspector.Target = m_Target;
					s_CurrentInspector.OnTargetChanged();
				}
			}
		}

		private static Dictionary<Type, CustomInspector> s_Inspectors = new Dictionary<Type, CustomInspector>();
		private static Dictionary<string, CustomInspector> s_FileExtensions = new Dictionary<string, CustomInspector>();

		[MenuItem("Window/Inspector")]
		private static void MenuCallback() => EditorUIService.Open<InspectorView>();

		protected override void Draw()
		{
			bool isOpen = true;
			if (ImGUI.Begin("Inspector", ref isOpen))
			{
				if (Target != null && s_CurrentInspector != null)
					s_CurrentInspector.DrawInspector();
				else if(Target != null)
					ImGUI.Text("No inspector found", Colour.Grey);
			}

			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<InspectorView>();
		}

		internal static void GetCustomInspectors()
		{
			s_Inspectors.Clear();

			// Get all methods with the MenuItem attribute, in all assemblies
			Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
			foreach (Assembly assembly in assemblies)
			{
				if (assembly.Location.Contains("Mono"))
					continue; // Don't check mono, system libs, and additional dependencies
				try
				{
					Type[] types = assembly.GetTypes();
					foreach (Type type in types)
					{
						CustomInspectorAttribute attribute = type.GetCustomAttribute<CustomInspectorAttribute>();
						if (attribute == null)
							continue;

						CustomInspector instance = Activator.CreateInstance(type) as CustomInspector;
						if (instance != null)
							AddCustomInspector(attribute, instance);
						else
							Log.Warning($"CustomInspector attribute is on class '{type.Name}', but this class does not inherit AquaEditor.CustomInspector");
					}
				}
				catch { }
			}
		}

		private static void AddCustomInspector(CustomInspectorAttribute attribute, CustomInspector inspector)
		{
			if (attribute.TargetType != null)
				s_Inspectors.Add(attribute.TargetType, inspector);

			if (attribute.FileExtensions == null)
				return;

			foreach(string extension in attribute.FileExtensions)
			{
				if(string.IsNullOrEmpty(extension)) continue;
				string finalExtension = extension.ToLower();
				if (finalExtension[0] != '.')
					finalExtension = $".{finalExtension}";
				s_FileExtensions.Add(finalExtension, inspector);
			}
		}

		/// <summary>
		/// Checks for valid inspector in <see cref="s_Inspectors"/> by type.
		/// Searches recursively down inheritance tree, from topmost down to base type
		/// </summary>
		private static CustomInspector FindInspector(Type type)
		{
			if(type == null) return null;
			if(s_Inspectors.ContainsKey(type))
				return s_Inspectors[type];
			return FindInspector(type.BaseType);
		}

		private static CustomInspector FindInspector(VFSFile file) =>
			string.IsNullOrEmpty(file.Extension) || !s_FileExtensions.ContainsKey(file.Extension) ?
				null : s_FileExtensions[file.Extension.ToLower()];
	}
}