using System;
using AquaEngine;
using AquaEditor.Views;
using System.Reflection;
using AquaEngine.Graphics;

namespace AquaEditor.Inspectors
{
	[CustomInspector(typeof(Entity))]
	public class EntityInspector : CustomInspector
	{
		private Entity m_Target;

		public override void Opened() => SceneManager.WorldChanged += OnWorldChanged;
		public override void Closed() => SceneManager.WorldChanged -= OnWorldChanged;

		public override void OnTargetChanged() => m_Target = Target as Entity;

		private void OnWorldChanged(World world, bool added)
		{
			Entity target = InspectorView.Target as Entity;
			if (target && target.World.Equals(world))
				InspectorView.Target = null;
		}

		public override void DrawInspector()
		{
			// Make sure entity still exists in world, hasn't been deleted
			if(!m_Target.World.HasEntity(m_Target.ID))
			{
				InspectorView.Target = null;
				return;
			}

			NameComponent nameComponent;
			if(!m_Target.TryGetComponent(out nameComponent))
			{
				nameComponent = m_Target.AddComponent<NameComponent>();
				nameComponent.Name = "Entity";
			}

			string name = nameComponent.Name;
			if (ImGUI.Input("##nameComponent", ref name))
				nameComponent.Name = name;

			Component[] components = m_Target.GetComponents();
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

			IVector2 contentRegion = ImGUI.ContentRegionAvailable;
			if (!ImGUI.BeginTable(type.Name, 2))
				return;

			ImGUI.TableSetupColumn("Key", ImGUI.TableColumnFlags.WidthFixed, contentRegion.x * 0.3f);
			ImGUI.TableSetupColumn("Value");
			ImGUI.TableHeadersRow();

			foreach (var field in fields)
			{
				bool show = field.IsPublic;
				if (field.IsPublic && field.GetCustomAttribute<HideInInspectorAttribute>() != null)
					show = false;
				if (field.GetCustomAttribute<ShowInInspectorAttribute>() != null)
					show = true;

				if (show)
					DrawObject(
						field.Name,
						field.FieldType,
						field.GetValue(component),
						component,
						field.SetValue,
						field.GetCustomAttribute<RangeAttribute>()
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

				if (canRead)
					DrawObject(
							property.Name,
							property.PropertyType,
							property.GetValue(component),
							component,
							canWrite ? property.SetValue : (Action<object, object>)null,
							property.GetCustomAttribute<RangeAttribute>()
						);
			}

			ImGUI.EndTable();
			ImGUI.Unindent();
		}

		private void Draw(string label, string value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Input($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, float value, object instance, Action<object, object> setValue, object additionalInfo)
		{
			RangeAttribute range = additionalInfo as RangeAttribute;
			if ((range != null && ImGUI.Slider($"##{label}", ref value, range.Range.x, range.Range.y)) ||
				(range == null && ImGUI.Drag($"##{label}", ref value)))
				setValue.Invoke(instance, value);
		}

		private void Draw(string label, int value, object instance, Action<object, object> setValue, object additionalInfo)
		{
			RangeAttribute range = additionalInfo as RangeAttribute;
			if ((range != null && ImGUI.Slider($"##{label}", ref value, (int)range.Range.x, (int)range.Range.y)) ||
				(range == null && ImGUI.Drag($"##{label}", ref value)))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, bool value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Checkbox($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Vector2 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Vector3 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Vector4 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}
		
		private void Draw(string label, Quaternion value, object instance, Action<object, object> setValue)
		{
			Vector3 euler = value.Euler;
			if (ImGUI.Drag($"##{label}", ref euler))
				setValue.Invoke(instance, Quaternion.FromEuler(euler));
		}
		
		private void Draw(string label, Colour value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.ColourEdit4($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		private const float TexturePreviewSize = 125;
		private Vector2 TextureDrawSize = new Vector2(50, 50);

		private void Draw(string label, Texture value, object instance, Action<object, object> setValue)
		{
			ImGUI.Image(value ?? EditorUIService.MissingTexture, TextureDrawSize);

			if (value != null && ImGUI.IsItemHovered() && ImGUI.BeginTooltip())
			{
				float aspectRatio = value.Resolution.x / (float)value.Resolution.y;
				ImGUI.Image(value,
					new Vector2(TexturePreviewSize * aspectRatio, TexturePreviewSize));
				ImGUI.EndTooltip();
			}
		}

		private void Draw(string label, RenderTexture value, object instance, Action<object, object> setValue)
		{
			if (value != null)
				ImGUI.Image(value, TextureDrawSize);
			else
				ImGUI.Image(EditorUIService.MissingTexture, TextureDrawSize);

			if (value != null && ImGUI.IsItemHovered() && ImGUI.BeginTooltip())
			{
				float aspectRatio = value.Resolution.x / (float)value.Resolution.y;
				ImGUI.Image(value,
					new Vector2(TexturePreviewSize * aspectRatio, TexturePreviewSize));
				ImGUI.EndTooltip();
			}
		}

		private void DrawEnum(string label, Type type, object value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.EnumCombo(label, type, ref value))
				setValue.Invoke(instance, value);
		}

		private void DrawObject(string label, Type t, object value, object instance, Action<object, object> setValue, object additionalInfo)
		{
			if (setValue == null) ImGUI.BeginDisabled();

			IVector2 region = ImGUI.ContentRegionAvailable;

			ImGUI.TableNextRow();
			ImGUI.TableSetColumnIndex(0);

			ImGUI.Text(label);

			ImGUI.TableSetColumnIndex(1);
			ImGUI.SetNextItemWidth(region.x);
			if (t	   == typeof(int))				Draw(label, (int)value,				instance, setValue, additionalInfo);
			else if (t == typeof(bool))				Draw(label, (bool)value,			instance, setValue);
			else if (t == typeof(float))			Draw(label, (float)value,			instance, setValue, additionalInfo);
			else if (t == typeof(string))			Draw(label, (string)value,			instance, setValue);
			else if (t == typeof(Colour))			Draw(label, (Colour)value,			instance, setValue);
			else if (t == typeof(Vector2))			Draw(label, (Vector2)value,			instance, setValue);
			else if (t == typeof(Vector3))			Draw(label, (Vector3)value,			instance, setValue);
			else if (t == typeof(Vector4))			Draw(label, (Vector4)value,			instance, setValue);
			else if (t == typeof(Texture))			Draw(label, (Texture)value,			instance, setValue);
			else if (t == typeof(Quaternion))		Draw(label, (Quaternion)value,		instance, setValue);
			else if (t == typeof(RenderTexture))	Draw(label, (RenderTexture)value,	instance, setValue);
			else if (t == typeof(UUID))
			{
				UUID uuid = (UUID)value;
				if (Resource.Exists(uuid))
					ImGUI.Text("[Resource] " + Resource.GetPath(uuid));
			}
			else if(t.IsEnum)
				DrawEnum(label, t, value, instance, setValue);
			else ImGUI.Text(t.Name);

			if (setValue == null) ImGUI.EndDisabled();
		}
	}
}