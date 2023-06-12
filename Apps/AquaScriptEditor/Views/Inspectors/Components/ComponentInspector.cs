using System;
using AquaEngine;
using System.Linq;
using System.Reflection;

namespace AquaEditor.Inspectors
{
	[CustomInspector(typeof(Component))]
	public class ComponentInspector : CustomInspector
	{
		private Component component;

		public override void OnTargetChanged() => component = (Component)Target;

		public override void DrawInspector()
		{
			Type type = component.GetType();
			var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
			var properties = type.GetProperties(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

			IVector2 contentRegion = (IVector2)ImGUI.ContentRegionAvailable;
			if (!ImGUI.BeginTable(type.Name, 2))
				return;

			ImGUI.TableSetupColumn("Key", ImGUI.TableColumnFlags.WidthFixed, contentRegion.x * 0.3f);
			ImGUI.TableSetupColumn("Value", ImGUI.TableColumnFlags.WidthStretch);

			foreach (var field in fields)
			{
				bool show = field.IsPublic;
				if (field.IsPublic && field.GetCustomAttribute<HideInInspectorAttribute>() != null)
					show = false;
				if (field.GetCustomAttribute<ShowInInspectorAttribute>() != null)
					show = true;

				SerializeAttribute serializeAttribute = field.GetCustomAttribute<SerializeAttribute>();

				if (show)
					DrawObject(
						string.IsNullOrEmpty(serializeAttribute?.Label) ? field.Name : serializeAttribute.Label,
						field.FieldType,
						field.GetValue(component),
						component,
						field.SetValue,
						field.GetCustomAttributes().ToArray()
					);
			}
			foreach (var property in properties)
			{
				if (property.GetCustomAttribute<HideInInspectorAttribute>() != null)
					continue;

				MethodInfo[] accessors = property.GetAccessors(true);
				bool canRead = accessors.Length > 0 && accessors[0].IsPublic;
				if (property.GetCustomAttribute<ShowInInspectorAttribute>() != null)
					canRead = true;

				bool canWrite = property.CanWrite && accessors.Length > 1 && accessors[1].IsPublic;
				if (accessors.Length > 1 && accessors[1].GetCustomAttribute<ShowInInspectorAttribute>() != null)
					canWrite = true;

				SerializeAttribute serializeAttribute = property.GetCustomAttribute<SerializeAttribute>();

				if (canRead)
					DrawObject(
							string.IsNullOrEmpty(serializeAttribute?.Label) ? property.Name : serializeAttribute.Label,
							property.PropertyType,
							property.GetValue(component),
							component,
							canWrite ? property.SetValue : (Action<object, object>)null,
							property.GetCustomAttributes().ToArray()
						);
			}

			ImGUI.EndTable();
		}
	}
}