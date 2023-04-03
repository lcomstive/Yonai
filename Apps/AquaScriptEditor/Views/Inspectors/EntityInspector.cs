using System;
using AquaEngine;
using System.Reflection;
using System.Linq;
using System.Collections.Generic;
using AquaEngine.Graphics;

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
			var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
			var properties = type.GetProperties(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

			if (!ImGUI.Foldout(type.Name, true))
				return;
			ImGUI.Indent();

			foreach (var field in fields)
			{
				bool show = true;
				if (field.IsPublic && field.GetCustomAttribute<HideInInspectorAttribute>() != null)
					show = false;
				if (field.IsPrivate && field.GetCustomAttribute<ShowInInspectorAttribute>() == null)
					show = false;

				if (show)
					DrawObject(field.Name, field.GetValue(component), component, field.SetValue);
			}
			foreach (var property in properties)
			{
				if (property.GetCustomAttribute<HideInInspectorAttribute>() != null)
					continue;

				DrawObject(
					property.Name,
					property.GetValue(component),
					component,
					property.CanWrite ? property.SetValue : (Action<object, object>)null
				);
			}

			ImGUI.Unindent();
		}

		private void Draw(string label, string value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Input(label, ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, float value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag(label, ref value))
				setValue.Invoke(instance, value);
		}	
		
		private void Draw(string label, int value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag(label, ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, bool value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Checkbox(label, ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Vector2 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag(label, ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Vector3 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag(label, ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Vector4 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag(label, ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Quaternion value, object instance, Action<object, object> setValue)
		{
			Vector3 euler = value.Euler;
			if (ImGUI.Drag(label, ref euler))
				setValue.Invoke(instance, Quaternion.FromEuler(euler));
		}
		
		private void Draw(string label, Colour value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.ColourEdit4(label, ref value))
				setValue.Invoke(instance, value);
		}

		private void DrawObject(string label, object value, object instance, Action<object, object> setValue)
		{
			if (value == null)
				return;

			Type t = value.GetType();

			if (setValue == null) ImGUI.BeginDisabled();

			if (t		== typeof(int))			Draw(label, (int)value,			instance, setValue);
			if (t		== typeof(bool))		Draw(label, (bool)value,		instance, setValue);
			if (t		== typeof(float))		Draw(label, (float)value,		instance, setValue);
			if (t		== typeof(string))		Draw(label, (string)value,		instance, setValue);
			else if (t	== typeof(Colour))		Draw(label, (Colour)value,		instance, setValue);
			else if (t	== typeof(Vector2))		Draw(label, (Vector2)value,		instance, setValue);
			else if (t	== typeof(Vector3))		Draw(label, (Vector3)value,		instance, setValue);
			else if (t	== typeof(Vector4))		Draw(label, (Vector4)value,		instance, setValue);
			else if (t	== typeof(Quaternion))	Draw(label, (Quaternion)value,	instance, setValue);

			if (setValue == null) ImGUI.EndDisabled();
		}
	}
}