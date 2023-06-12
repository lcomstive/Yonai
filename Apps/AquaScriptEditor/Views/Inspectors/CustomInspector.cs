
using AquaEditor.Views;
using AquaEngine.Graphics;
using AquaEngine;
using System;

namespace AquaEditor
{
	public abstract class CustomInspector
	{
		public object Target { get; internal set; }

		/// <summary>
		/// Called when this inspector is opened
		/// </summary>
		public virtual void Opened() { }

		/// <summary>
		/// Called when this inspector is closed
		/// </summary>
		public virtual void Closed() { }

		/// <summary>
		/// Called whenever <see cref="Target"/> gets set
		/// </summary>
		public virtual void OnTargetChanged() { }

		/// <summary>
		/// Called once per frame
		/// </summary>
		public abstract void DrawInspector();

		#region Drawing
		protected void Draw(string label, string value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Input($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, float value, object instance, Action<object, object> setValue, Attribute[] attributes)
		{
			// Check for modifying attributes
			for (int i = 0; i < attributes.Length; i++)
			{
				if (attributes[i] is RangeAttribute)
				{
					RangeAttribute rangeAttribute = (RangeAttribute)attributes[i];
					if (ImGUI.Slider($"##{label}", ref value, rangeAttribute.Range.x, rangeAttribute.Range.y))
						setValue.Invoke(instance, value);
					return;
				}
			}

			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, int value, object instance, Action<object, object> setValue, Attribute[] attributes)
		{
			// Check for modifying attributes
			for (int i = 0; i < attributes.Length; i++)
			{
				if (attributes[i] is RangeAttribute)
				{
					RangeAttribute rangeAttribute = (RangeAttribute)attributes[i];
					if (ImGUI.Slider($"##{label}", ref value, (int)rangeAttribute.Range.x, (int)rangeAttribute.Range.y))
						setValue.Invoke(instance, value);
					return;
				}
			}

			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, bool value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Checkbox($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, Vector2 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, Vector3 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, Vector4 value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Drag($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, Quaternion value, object instance, Action<object, object> setValue)
		{
			Vector3 euler = value.Euler;
			if (ImGUI.Drag($"##{label}", ref euler))
				setValue.Invoke(instance, Quaternion.FromEuler(euler));
		}

		protected void Draw(string label, Colour value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.ColourEdit4($"##{label}", ref value))
				setValue.Invoke(instance, value);
		}

		protected const float TexturePreviewSize = 125;
		protected readonly Vector2 TextureDrawSize = new Vector2(50, 50);

		protected void Draw(string label, Texture value, object instance, Action<object, object> setValue)
		{
			ImGUI.Image(value ?? EditorUIService.MissingTexture, TextureDrawSize);

			if (value != null && ImGUI.IsItemHovered() && ImGUI.BeginTooltip())
			{
				float aspectRatio = value.Resolution.x / (float)value.Resolution.y;
				ImGUI.Image(value,
					new Vector2(TexturePreviewSize * aspectRatio, TexturePreviewSize));
				ImGUI.EndTooltip();
			}

			if (ImGUI.IsItemHovered() && ImGUI.IsMouseDoubleClicked(MouseButton.Left))
			{
				// Navigate resources view to resource.ResourcePath and highlight/select resource
				ResourcesView resourceView = EditorUIService.GetView<ResourcesView>();
				if (resourceView == null)
					resourceView = EditorUIService.Open<ResourcesView>();
				resourceView.HighlightPath(value.ResourcePath);
			}

			UUID dragDropID = HandleResourceDragDrop(value);
			if (dragDropID != UUID.Invalid)
				setValue.Invoke(instance, Resource.Get(dragDropID));
		}

		protected void Draw(string label, RenderTexture value, object instance, Action<object, object> setValue)
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

		protected void DrawEnum(string label, Type type, object value, object instance, Action<object, object> setValue)
		{
			if (ImGUI.EnumCombo(label, type, ref value))
				setValue.Invoke(instance, value);
		}

		protected void Draw(string label, UUID value, object instance, Action<object, object> setValue)
		{
			if (Resource.Exists(value))
				ImGUI.Text(Resource.GetPath(value));
			else
				ImGUI.Text(value.ToString());
		}

		protected void Draw(string label, ResourceBase resource, object instance, Action<object, object> setValue)
		{
			if (ImGUI.Button(resource.ResourcePath) && ImGUI.IsMouseDoubleClicked(MouseButton.Left))
			{
				// Navigate resources view to resource.ResourcePath and highlight/select resource
				ResourcesView resourceView = EditorUIService.GetView<ResourcesView>();
				if (resourceView == null)
					resourceView = EditorUIService.Open<ResourcesView>();
				resourceView.HighlightPath(resource.ResourcePath);
			}

			UUID dragDropID = HandleResourceDragDrop(resource);
			if (dragDropID != UUID.Invalid)
				setValue.Invoke(instance, Resource.Get(dragDropID));
		}

		protected UUID HandleResourceDragDrop(ResourceBase resource)
		{
			if (ImGUI.BeginDragDropTarget())
			{
				object payload = ImGUI.AcceptDragDropPayload("ResourcePath", ImGUI.DragDropFlags.AcceptPeekOnly);
				if (payload == null)
					return UUID.Invalid;
				VFSFile payloadFile = (VFSFile)payload;
				if (!Resource.Exists(payloadFile.FullPath))
					return UUID.Invalid;
				UUID resourceID = Resource.GetID(payloadFile.FullPath);
				Type payloadType = Resource.GetType(resourceID);

				if (resource.GetType().IsAssignableFrom(payloadType))
				{
					ImGUI.AcceptDragDropPayload("ResourcePath");
					if (ImGUI.DragDropPayloadIsDelivery())
						return resourceID;
				}

				ImGUI.EndDragDropTarget();
			}
			return UUID.Invalid;
		}

		protected void DrawObject(string label, Type t, object value, object instance, Action<object, object> setValue, Attribute[] attributes)
		{
			if (setValue == null) ImGUI.BeginDisabled();

			Vector2 region = ImGUI.ContentRegionAvailable;

			ImGUI.TableNextRow();
			ImGUI.TableSetColumnIndex(0);

			ImGUI.Text(label);

			ImGUI.TableSetColumnIndex(1);
			ImGUI.SetNextItemWidth(region.x);
			if (t == typeof(int)) Draw(label, (int)value, instance, setValue, attributes);
			else if (t == typeof(bool)) Draw(label, (bool)value, instance, setValue);
			else if (t == typeof(float)) Draw(label, (float)value, instance, setValue, attributes);
			else if (t == typeof(string)) Draw(label, (string)value, instance, setValue);
			else if (t == typeof(Colour)) Draw(label, (Colour)value, instance, setValue);
			else if (t == typeof(Vector2)) Draw(label, (Vector2)value, instance, setValue);
			else if (t == typeof(Vector3)) Draw(label, (Vector3)value, instance, setValue);
			else if (t == typeof(Vector4)) Draw(label, (Vector4)value, instance, setValue);
			else if (t == typeof(Texture)) Draw(label, (Texture)value, instance, setValue);
			else if (t == typeof(Quaternion)) Draw(label, (Quaternion)value, instance, setValue);
			else if (t == typeof(RenderTexture)) Draw(label, (RenderTexture)value, instance, setValue);
			else if (t == typeof(UUID)) Draw(label, (UUID)value, instance, setValue);
			else if (typeof(ResourceBase).IsAssignableFrom(t)) Draw(label, (ResourceBase)value, instance, setValue);
			else if (t.IsEnum)
				DrawEnum(label, t, value, instance, setValue);
			else ImGUI.Text(t.Name);

			if (setValue == null) ImGUI.EndDisabled();
		}
		#endregion
	}
}