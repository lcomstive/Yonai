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
				if (previousType == newType)
					return; // Nothing further needs to be done

				// Inform existing inspector of closure
				s_CurrentInspector?.Closed();

				while (newType != null)
				{
					if (s_Inspectors.ContainsKey(newType))
					{
						Log.Debug("Found valid inspector");
						s_CurrentInspector = s_Inspectors[newType];
						s_CurrentInspector.Opened();
						break;
					}
					else
					{
						Log.Debug("No inspector found for this type");
						s_CurrentInspector = null;
					}

					newType = newType.BaseType;
				}
			}
		}

		private static Dictionary<Type, CustomInspector> s_Inspectors = new Dictionary<Type, CustomInspector>();

		[MenuItem("Window/Inspector")]
		private static void MenuCallback() => EditorUIService.Open<InspectorView>();

		protected override void Draw()
		{
			bool isOpen = true;
			if (ImGUI.Begin("Inspector", ref isOpen))
			{
				if (Target != null && s_CurrentInspector != null)
					s_CurrentInspector.DrawInspector(Target);
				else
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
						{
							s_Inspectors.Add(attribute.TargetType, instance);
							Log.Info($"Inspector added for '{type.Name}' -> '{attribute.TargetType.Name}'");
						}
						else
							Log.Warning($"CustomInspector attribute is on class '{type.Name}', but this class does not inherit AquaEditor.CustomInspector");
					}
				}
				catch { }
			}
		}
	}
}