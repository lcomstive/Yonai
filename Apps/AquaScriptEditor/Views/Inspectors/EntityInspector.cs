using System;
using AquaEngine;
using System.Reflection;
using System.Linq;

namespace AquaEditor
{
	[CustomInspector(typeof(Entity))]
	public class EntityInspector : CustomInspector
	{
		public override void DrawInspector(object target)
		{
			Entity entity = (Entity)target;

			ImGUI.Text($"[{entity.World.ID}:{entity.ID}]", Colour.Grey);
			if(entity.TryGetComponent(out NameComponent nameComponent))
			{
				ImGUI.SameLine();
				string name = nameComponent.Name;
				if (ImGUI.Input("##nameComponent", ref name))
					nameComponent.Name = name;
			}

			Component[] components = entity.GetComponents();
			foreach(Component component in components)
				DrawComponentInspector(component);
		}

		private void DrawComponentInspector(Component component)
		{
			if (component is NameComponent)
				return; // Skip name component


			Type type = component.GetType();
			BindingFlags flags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance;
			var fields = type.GetFields(flags);
			var properties = type.GetProperties();
			properties.Concat(type.GetProperties(BindingFlags.NonPublic | BindingFlags.Instance));

			if (!ImGUI.Foldout(type.Name, true))
				return;

			foreach (var field in fields)
			{
				bool show = true;
				if (field.IsPublic && field.GetCustomAttribute<HideInInspectorAttribute>() != null)
					show = false;
				if (field.IsPrivate && field.GetCustomAttribute<ShowInInspectorAttribute>() == null)
					show = false;

				if (show)
					ImGUI.Text(field.Name);
			}
			foreach (var property in properties)
			{
				if (property.GetCustomAttribute<HideInInspectorAttribute>() != null)
					continue;

				if (property.CanWrite)
					ImGUI.Text(property.Name);
				else
					ImGUI.Text(property.Name + " [ReadOnly]");
			}
		}
	}
}