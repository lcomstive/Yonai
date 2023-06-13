using System;
using AquaEngine;
using System.Linq;
using AquaEngine.IO;
using AquaEditor.Views;
using System.Reflection;
using AquaEngine.Graphics;

namespace AquaEditor
{
	public class CustomInspector
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
		public virtual void DrawInspector()
		{
			SetupTable();
			DrawInspector(Target);
			ImGUI.EndTable();
		}

		/// <summary>
		/// Sets up a 2-column table. Left for label, right for value.
		/// <b>Don't forget to call <see cref="ImGUI.EndTable"/>.</b>
		/// </summary>
		protected void SetupTable()
		{
			IVector2 contentRegion = (IVector2)ImGUI.ContentRegionAvailable;
			if (!ImGUI.BeginTable(Target.GetType().Name, 2))
				return;

			ImGUI.TableSetupColumn("Key", ImGUI.TableColumnFlags.WidthFixed, contentRegion.x * 0.3f);
			ImGUI.TableSetupColumn("Value");
		}

		public void DrawInspector(object target)
		{
			if (target == null)
				return;

			Type type = target.GetType();
			var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
			var properties = type.GetProperties(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

			foreach (var field in fields)
			{
				bool show = field.IsPublic;
				if (field.IsPublic && field.GetCustomAttribute<HideInInspectorAttribute>() != null)
					show = false;
				if (field.GetCustomAttribute<ShowInInspectorAttribute>() != null)
					show = true;

				SerializeAttribute serializeAttribute = field.GetCustomAttribute<SerializeAttribute>();
				show = show && (serializeAttribute?.ShouldSerialize ?? true);

				if (!show)
					continue;

				field.SetValue(
					target,
					DrawObject(
						string.IsNullOrEmpty(serializeAttribute?.Label) ? field.Name : serializeAttribute.Label,
						field.FieldType,
						field.GetValue(target),
						field.GetCustomAttributes().ToArray()
				));
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

				if (!canRead || (!serializeAttribute?.ShouldSerialize ?? false))
					continue;

				if (!canWrite)
					ImGUI.BeginDisabled();
				object value = DrawObject(
						string.IsNullOrEmpty(serializeAttribute?.Label) ? property.Name : serializeAttribute.Label,
						property.PropertyType,
						property.GetValue(target),
						property.GetCustomAttributes().ToArray()
					);

				if (canWrite)
					property.SetValue(target, value);
				else
					ImGUI.EndDisabled();
			}
		}

		#region Drawing
		private string Draw(string label, string value)
		{
			ImGUI.Input($"##{label}", ref value);
			return value;
		}

		private float Draw(string label, float value, Attribute[] attributes = null)
		{
			// Check for modifying attributes
			for (int i = 0; i < attributes.Length; i++)
			{
				if (attributes[i] is RangeAttribute)
				{
					RangeAttribute rangeAttribute = (RangeAttribute)attributes[i];
					ImGUI.Slider($"##{label}", ref value, rangeAttribute.Range.x, rangeAttribute.Range.y);
					return value;
				}
			}

			ImGUI.Drag($"##{label}", ref value);
			return value;
		}

		private int Draw(string label, int value, Attribute[] attributes = null)
		{
			// Check for modifying attributes
			for (int i = 0; i < attributes.Length; i++)
			{
				if (attributes[i] is RangeAttribute)
				{
					RangeAttribute rangeAttribute = (RangeAttribute)attributes[i];
					ImGUI.Slider($"##{label}", ref value, (int)rangeAttribute.Range.x, (int)rangeAttribute.Range.y);
					return value;
				}
			}

			ImGUI.Drag($"##{label}", ref value);
			return value;
		}

		private bool Draw(string label, bool value)
		{
			ImGUI.Checkbox($"##{label}", ref value);
			return value;
		}

		private Vector2 Draw(string label, Vector2 value)
		{
			ImGUI.Drag($"##{label}", ref value);
			return value;
		}

		private Vector3 Draw(string label, Vector3 value, string format = "%.2f")
		{
			ImGUI.Drag($"##{label}", ref value, 1, 0, 0, "%.2f");
			return value;
		}

		private Vector4 Draw(string label, Vector4 value)
		{
			ImGUI.Drag($"##{label}", ref value);
			return value;
		}

		private Quaternion Draw(string label, Quaternion value)
		{
			Vector3 euler = value.Euler;
			ImGUI.Drag($"##{label}", ref euler);
			return Quaternion.FromEuler(euler);
		}

		private Colour Draw(string label, Colour value)
		{
			ImGUI.ColourEdit4($"##{label}", ref value);
			return value;
		}

		private Texture Draw(string label, Texture value, float previewSize = 125) => Draw(label, value, new Vector2(50, 50), previewSize);
		private Texture Draw(string label, Texture value, Vector2 drawSize, float previewSize = 125)
		{
			ImGUI.Image(value ?? EditorUIService.MissingTexture, drawSize);

			if (value != null && ImGUI.IsItemHovered() && ImGUI.BeginTooltip())
			{
				float aspectRatio = value.Resolution.x / (float)value.Resolution.y;
				ImGUI.Image(value,
					new Vector2(previewSize * aspectRatio, previewSize));
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

			UUID dragDropID = HandleResourceDragDrop(typeof(Texture));
			if (dragDropID != UUID.Invalid)
				return Resource.Get<Texture>(dragDropID);
			return value;
		}

		private RenderTexture Draw(string label, RenderTexture value, float previewSize = 125) => Draw(label, value, new Vector2(50, 50), previewSize);
		private RenderTexture Draw(string label, RenderTexture value, Vector2 textureDrawSize, float previewSize = 125)
		{
			if (value != null)
				ImGUI.Image(value, textureDrawSize);
			else
				ImGUI.Image(EditorUIService.MissingTexture, textureDrawSize);

			if (value != null && ImGUI.IsItemHovered() && ImGUI.BeginTooltip())
			{
				float aspectRatio = value.Resolution.x / (float)value.Resolution.y;
				ImGUI.Image(value,
					new Vector2(previewSize * aspectRatio, previewSize));
				ImGUI.EndTooltip();
			}

			return value;
		}

		private T DrawEnum<T>(string label, T value) where T : Enum => (T)DrawEnum(label, typeof(T), value);

		private object DrawEnum(string label, Type type, object value)
		{
			ImGUI.EnumCombo(label, type, ref value);
			return value;
		}

		private void Draw(string label, UUID value)
		{
			if (Resource.Exists(value))
				ImGUI.Text(Resource.GetPath(value));
			else
				ImGUI.Text(value.ToString());
		}

		private ResourceBase Draw(string label, Type type, ResourceBase resource)
		{
			if (ImGUI.Button(resource?.ResourcePath ?? string.Empty, new Vector2(ImGUI.ContentRegionAvailable.x, 0))
					&& resource != null && ImGUI.IsMouseDoubleClicked(MouseButton.Left))
			{
				// Navigate resources view to resource.ResourcePath and highlight/select resource
				ResourcesView resourceView = EditorUIService.GetView<ResourcesView>();
				if (resourceView == null)
					resourceView = EditorUIService.Open<ResourcesView>();
				resourceView.HighlightPath(resource.ResourcePath);
			}

			UUID dragDropID = HandleResourceDragDrop(type);
			if (dragDropID != UUID.Invalid)
				return Resource.Get(dragDropID);

			if (ImGUI.BeginPopupContextItem(label + ":ContextMenu", ImGUI.PopupFlags.MouseButtonRight))
			{
				if (ImGUI.Selectable("Clear"))
					resource = null;
				if(resource != null && ImGUI.Selectable("Copy Path"))
					Clipboard.SetText(resource.ResourcePath);
				if(ImGUI.Selectable("Paste Path"))
				{
					string path = Clipboard.GetText();
					if (!string.IsNullOrEmpty(path))
						resource = Resource.Load(Resource.GetID(path), path, type);
				}
				ImGUI.EndPopup();
			}

			return resource;
		}

		private ResourceBase DrawResource(string label, UUID resourceID)
		{
			ResourceBase value = Resource.Get(resourceID);
			return value ? Draw(label, value.GetType(), value) : null;
		}

		private UUID HandleResourceDragDrop(Type type)
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

				if (type.IsAssignableFrom(payloadType))
				{
					ImGUI.AcceptDragDropPayload("ResourcePath");
					if (ImGUI.DragDropPayloadIsDelivery())
						return resourceID;
				}

				ImGUI.EndDragDropTarget();
			}
			return UUID.Invalid;
		}

		protected T Draw<T>(string label, T value, Attribute attribute) => Draw(label, value, new Attribute[] { attribute });
		protected T Draw<T>(string label, T value, Attribute[] attributes = null) => (T)DrawObject(label, typeof(T), value, attributes);

		protected UUID DrawResource<T>(string label, UUID resourceID) where T : ResourceBase => ((T)DrawObject(label, typeof(T), Resource.Get(resourceID)))?.ResourceID ?? 0;

		/// <summary>
		/// Draws an object inspector
		/// </summary>
		/// <param name="label">Label shown to user</param>
		/// <param name="t">Type of object to be drawn</param>
		/// <param name="value">Current value of object</param>
		/// <param name="attributes">Attributes to modify how some objects are drawn (e.g. <see cref="RangeAttribute"/>)</param>
		protected object DrawObject(string label, Type t, object value, Attribute[] attributes = null)
		{
			ImGUI.TableNextRow();
			ImGUI.TableSetColumnIndex(0);

			ImGUI.Text(label);
			if (ImGUI.IsItemHovered(ImGUI.HoveredFlags.DelayNormal | ImGUI.HoveredFlags.NoSharedDelay))
				ImGUI.SetTooltip(label);

			ImGUI.TableSetColumnIndex(1);
			if (t == typeof(int)) return Draw(label, (int)value, attributes);
			else if (t == typeof(bool)) return Draw(label, (bool)value);
			else if (t == typeof(float)) return Draw(label, (float)value, attributes);
			else if (t == typeof(string)) return Draw(label, (string)value);
			else if (t == typeof(Colour)) return Draw(label, (Colour)value);
			else if (t == typeof(Vector2)) return Draw(label, (Vector2)value);
			else if (t == typeof(Vector3)) return Draw(label, (Vector3)value);
			else if (t == typeof(Vector4)) return Draw(label, (Vector4)value);
			else if (t == typeof(Texture)) return Draw(label, (Texture)value);
			else if (t == typeof(Quaternion)) return Draw(label, (Quaternion)value);
			else if (t == typeof(RenderTexture)) return Draw(label, (RenderTexture)value);
			else if (t == typeof(UUID))
			{
				UUID uuid = (UUID)value;
				if (Resource.Exists(uuid))
					return DrawResource(label, uuid)?.ResourceID ?? UUID.Invalid;
			
				Draw(label, (UUID)value);
				return value;
			}
			else if (typeof(ResourceBase).IsAssignableFrom(t)) return Draw(label, t, (ResourceBase)value);
			else if (t.IsEnum)
				return DrawEnum(label, t, value);

			// Default
			ImGUI.Text(t.Name, Colour.Grey);
			return value;
		}
		#endregion
	}
}