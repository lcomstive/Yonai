using System;
using System.Reflection;
using Newtonsoft.Json.Linq;

namespace Yonai.IO
{
	public interface ISerializable
	{
		/// <summary>
		/// Called when serializing this object
		/// </summary>
		JObject OnSerialize();

		/// <summary>
		/// Called when deserializing this object
		/// </summary>
		void OnDeserialize(JObject json);
	}

	public static class SerializableHelper
	{
		/// <summary>
		/// Can be used as a default for <see cref="ISerializable.OnSerialize"/>.
		/// Scans all fields and properties, saving any public or marked with <see cref="SerializeAttribute"/>.
		/// </summary>
		/// <returns>Serialized object of <paramref name="instance"/></returns>
		public static JObject OnSerializeDefault(ISerializable instance)
		{
			JObject json = new JObject();
			Type type = instance.GetType();

			var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
			var properties = type.GetProperties(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

			foreach (var field in fields)
			{
				if (ShouldSerializeField(field, out string label))
					SerializeObject(
						json,
						label,
						field.FieldType,
						field.GetValue(instance)
					);
			}
			foreach (var property in properties)
			{
				try
				{
					if (ShouldSerializeProperty(property, out string label))
						SerializeObject(
							json,
							label,
							property.PropertyType,
							property.GetValue(instance)
						);
				}
				catch (Exception e) { Log.Exception(e, $"Serializing {type.Name}"); }
			}

			return json;
		}

		public static void OnDeserializeDefault(ISerializable instance, JObject json)
		{
			Type type = instance.GetType();
			var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
			var properties = type.GetProperties(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

			Type serializableType = typeof(ISerializable);
			Type resourceBaseType = typeof(ResourceBase);

			foreach (var field in fields)
			{
				if (!ShouldSerializeField(field, out string label))
					continue;

				if (serializableType.IsAssignableFrom(field.FieldType) && !resourceBaseType.IsAssignableFrom(field.FieldType))
				{
					ISerializable serializable = (ISerializable)field.GetValue(instance);
					if (json[label] != null)
						serializable.OnDeserialize(json[label].Value<JObject>());
					field.SetValue(instance, serializable);
				}
				else
					field.SetValue(instance,
						DeserializeObject(
							json,
							label,
							field.FieldType
						));
			}
			foreach (var property in properties)
			{
				if (!ShouldSerializeProperty(property, out string label))
					continue;

				if (!serializableType.IsAssignableFrom(property.PropertyType) || resourceBaseType.IsAssignableFrom(property.PropertyType))
				{
					object value = DeserializeObject(
							json,
							label,
							property.PropertyType
						);
					if (value != null)
						property.SetValue(instance, value);
				}
				else
				{
					ISerializable serializable = (ISerializable)property.GetValue(instance);
					if (json[label] != null)
						serializable.OnDeserialize(json[label].Value<JObject>());
					property.SetValue(instance, serializable);
				}
			}
		}

		/// <param name="label">Label to use when serializing this field</param>
		/// <returns>
		/// True if <paramref name="field"/> is public or has <see cref="SerializeAttribute"/>.
		/// False if private, <see cref="SerializeAttribute.ShouldSerialize"/> is false,
		/// or <see cref="HideInInspectorAttribute"/> is present.
		/// </returns>
		public static bool ShouldSerializeField(FieldInfo field, out string label)
		{
			SerializeAttribute shouldSerializeAttr = field.GetCustomAttribute<SerializeAttribute>();
			label = field.Name;
			if (shouldSerializeAttr != null && !shouldSerializeAttr.ShouldSerialize)
				return false;
			if (shouldSerializeAttr == null && field.IsPublic && field.GetCustomAttribute<HideInInspectorAttribute>() != null)
				return false;

			label = string.IsNullOrEmpty(shouldSerializeAttr?.Label) ? field.Name : shouldSerializeAttr.Label;

			if (field.GetCustomAttribute<ShowInInspectorAttribute>() != null)
				return true;

			return field.IsPublic || shouldSerializeAttr != null;
		}

		/// <param name="label">Label to use when serializing this field</param>
		/// <returns>
		/// True if <paramref name="property"/> is public or has <see cref="SerializeAttribute"/>.
		/// False if private, has no setter, <see cref="SerializeAttribute.ShouldSerialize"/> is false,
		/// or <see cref="HideInInspectorAttribute"/> is present.
		/// </returns>
		public static bool ShouldSerializeProperty(PropertyInfo property, out string label)
		{
			label = property.Name;
			SerializeAttribute shouldSerializeAttr = property.GetCustomAttribute<SerializeAttribute>();
			if (shouldSerializeAttr != null)
				label = string.IsNullOrEmpty(shouldSerializeAttr.Label) ? property.Name : shouldSerializeAttr.Label;

			if (shouldSerializeAttr != null && !shouldSerializeAttr.ShouldSerialize)
				return false;
			else if (shouldSerializeAttr?.ShouldSerialize ?? false)
				return true;

			if (shouldSerializeAttr == null && property.GetCustomAttribute<HideInInspectorAttribute>() != null)
				return false;

			MethodInfo[] accessors = property.GetAccessors(true);
			bool canRead = false;
			bool canWrite = false;

			foreach (MethodInfo accessor in accessors)
			{
				if (accessor.Name.StartsWith("set_") && accessor.IsPublic)
					canWrite = true;
				if (accessor.Name.StartsWith("get_") && accessor.IsPublic)
					canRead = true;
			}

			if (!canWrite)
				return false;

			return canRead;
		}

		private static readonly Type ResourceBaseType = typeof(ResourceBase);
		/// <summary>
		/// Serializes common types to a JSON object
		/// </summary>
		public static void SerializeObject(JObject json, string label, Type t, object value)
		{
			if (value == null)
			{
				json[label] = null;
				return;
			}
			// Check if resource, if so then serialize as UUID
			if (ResourceBaseType.IsAssignableFrom(t))
				json[label] = ((ResourceBase)value).ResourceID.ToString();
			else if (typeof(ISerializable).IsAssignableFrom(t))
			{
				try { json[label] = ((ISerializable)value)?.OnSerialize(); }
				catch (Exception e)
				{
					Log.Exception(e, $"{label} ({t.Name})");
				}
			}
			// Reference to another component
			else if (typeof(Component).IsAssignableFrom(t))
			{
				if (value == null)
					return;
				json[label] = new JObject(
					new JProperty("Entity", ((Component)value).Entity.ID.ToString()),
					new JProperty("Type", $"{t.FullName}, {t.Assembly.GetName()}")
					);
			}
			else if (t == typeof(int)) json[label] = (int)value;
			else if (t == typeof(uint)) json[label] = (uint)value;
			else if (t == typeof(bool)) json[label] = (bool)value;
			else if (t == typeof(float)) json[label] = (float)value;
			else if (t == typeof(string)) json[label] = (string)value;
			else if (t == typeof(UUID)) json[label] = ((UUID)value).ToString();
			else if (t.IsEnum) json[label] = Enum.GetName(t, value);
			else
				Log.Warning($"Type '{t.Name}' has no method to be serialized");
		}

		/// <summary>
		/// Deserializes common types from a JSON object
		/// </summary>
		/// <returns>Deserialized object, or null if failed to parse</returns>
		public static object DeserializeObject(JObject json, string label, Type t)
		{
			try
			{
				if (json[label] == null) return null;

				if (typeof(ResourceBase).IsAssignableFrom(t)) return Resource.Get(ulong.Parse(json[label].Value<string>() ?? UUID.Invalid.ToString()));
				else if (t == typeof(int)) return json[label].Value<int>();
				else if (t == typeof(uint)) return json[label].Value<uint>();
				else if (t == typeof(bool)) return json[label].Value<bool>();
				else if (t == typeof(float)) return json[label].Value<float>();
				else if (t == typeof(string)) return json[label].Value<string>();
				else if (t == typeof(UUID)) return (UUID)ulong.Parse(json[label].Value<string>());
				else if (t.IsEnum) return Enum.Parse(t, json[label].Value<string>());
				else
					Log.Warning($"Type '{t.Name}' has no method to be deserialized");
			}
			catch (Exception e) { Log.Exception(e, $"Failed to deserialize '{label}' on type '{t.Name}'"); }
			return null;
		}
	}
}