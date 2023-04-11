﻿using System;
using AquaEngine.IO;
using System.Reflection;
using Newtonsoft.Json.Linq;

namespace AquaEngine
{
	public abstract class Component : ISerializable
	{
		internal IntPtr Handle { get; set; }

		/// <summary>
		/// The <see cref="World"/> this component exists within.
		/// This matches the <see cref="World"/> of the attached <see cref="Entity"/>.
		/// </summary>
		[HideInInspector]
		public World World => Entity.World;

		/// <summary>
		/// <see cref="Entity"/> this component is attached to.
		/// </summary>
		[HideInInspector]
		public Entity Entity { get; private set; }

		/// <summary>
		/// Returns true if object is not null
		/// </summary>
		public static implicit operator bool(Component component) => !ReferenceEquals(component, null);

		public virtual JObject OnSerialize()
		{
			JObject json = new JObject();
			Type type = GetType();

			json["ComponentType"] = type.FullName;
			if (type.Namespace != "AquaEngine")
				json["ComponentType"] += ", " + type.Assembly.GetName();

			var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
			var properties = type.GetProperties(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

			foreach (var field in fields)
			{
				if(ShouldSerializeField(field))
					SerializeObject(
						json,
						field.Name,
						field.FieldType,
						field.GetValue(this)
					);
			}
			foreach (var property in properties)
			{
				if (ShouldSerializeProperty(property))
					SerializeObject(
						json,
						property.Name,
						property.PropertyType,
						property.GetValue(this)
					);
			}

			return json;
		}

		public virtual void OnDeserialize(JObject json)
		{
			Type type = GetType();
			var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
			var properties = type.GetProperties(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

			Type serializableType = typeof(ISerializable);
			Type resourceBaseType = typeof(ResourceBase);

			foreach (var field in fields)
			{
				if (!ShouldSerializeField(field))
					continue;

				if(serializableType.IsAssignableFrom(field.FieldType) && !resourceBaseType.IsAssignableFrom(field.FieldType))
				{
					ISerializable serializable = (ISerializable)field.GetValue(this);
					serializable.OnDeserialize(json[field.Name].Value<JObject>());
					field.SetValue(this, serializable);
				}
				else
					field.SetValue(this,
						DeserializeObject(
							json,
							field.Name,
							field.FieldType
						));
			}
			foreach (var property in properties)
			{
				if (!ShouldSerializeProperty(property))
					continue;

				if (!serializableType.IsAssignableFrom(property.PropertyType) || resourceBaseType.IsAssignableFrom(property.PropertyType))
				{
					object value = DeserializeObject(
							json,
							property.Name,
							property.PropertyType
						);
					if (value != null)
						property.SetValue(this, value);
				}
				else
				{
					ISerializable serializable = (ISerializable)property.GetValue(this);
					serializable.OnDeserialize(json[property.Name].Value<JObject>());
					property.SetValue(this, serializable);
				}
			}
		}

		private bool ShouldSerializeField(FieldInfo field)
		{
			SerializeAttribute shouldSerializeAttr = field.GetCustomAttribute<SerializeAttribute>();
			if (shouldSerializeAttr != null && !shouldSerializeAttr.ShouldSerialize)
				return false;
			if (shouldSerializeAttr == null && field.IsPublic && field.GetCustomAttribute<HideInInspectorAttribute>() != null)
				return false;

			bool save = field.IsPublic;
			if (field.GetCustomAttribute<ShowInInspectorAttribute>() != null)
				return true;

			return field.IsPublic || shouldSerializeAttr != null;
		}

		private bool ShouldSerializeProperty(PropertyInfo property)
		{
			SerializeAttribute shouldSerializeAttr = property.GetCustomAttribute<SerializeAttribute>();
			if (shouldSerializeAttr != null && !shouldSerializeAttr.ShouldSerialize)
				return false;
			else if (shouldSerializeAttr?.ShouldSerialize ?? false)
				return true;

			if (shouldSerializeAttr == null && property.GetCustomAttribute<HideInInspectorAttribute>() != null)
				return false;

			MethodInfo[] accessors = property.GetAccessors(true);
			bool canRead  = false;
			bool canWrite = false;

			foreach(MethodInfo accessor in accessors)
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

		private void SerializeObject(JObject json, string label, Type t, object value)
		{
			// Check if resource, if so then serialize as UUID
			if(typeof(ResourceBase).IsAssignableFrom(t))
				json[label] = ((ResourceBase)value).ResourceID.ToString();
			else if (typeof(ISerializable).IsAssignableFrom(t))
			{
				try { json[label] = ((ISerializable)value).OnSerialize(); }
				catch(Exception e)
				{
					Log.Exception(e, $"{label} ({t.Name})");
				}
			}
			else if (t == typeof(int))		json[label] = (int)value;
			else if (t == typeof(uint))		json[label] = (uint)value;
			else if (t == typeof(bool))		json[label] = (bool)value;
			else if (t == typeof(float))	json[label] = (float)value;
			else if (t == typeof(string))	json[label] = (string)value;
			else if (t == typeof(UUID))		json[label] = ((UUID)value).ToString();
			else if (t.IsEnum)				json[label] = Enum.GetName(t, value);
			else
				Log.Debug($"Type '{t.Name}' has no method to be serialized");
		}
		
		private object DeserializeObject(JObject json, string label, Type t)
		{
			if (typeof(ResourceBase).IsAssignableFrom(t)) return Resource.Get(ulong.Parse(json[label].Value<string>()));
			if (t == typeof(int))			return json[label].Value<int>();
			else if (t == typeof(uint))		return json[label].Value<uint>();
			else if (t == typeof(bool))		return json[label].Value<bool>();
			else if (t == typeof(float))	return json[label].Value<float>();
			else if (t == typeof(string))	return json[label].Value<string>();
			else if (t == typeof(UUID))		return (UUID)ulong.Parse(json[label].Value<string>());
			else if (t.IsEnum)				return Enum.Parse(t, json[label].Value<string>());
			else
				Log.Debug($"Type '{t.Name}' has no method to be deserialized");
			return null;
		}

		#region Entity Proxy
		/// <returns>True if this entity has an attached <see cref="Component"/></returns>
		public bool HasComponent<T>() where T : Component => Entity?.HasComponent<T>() ?? false;

		/// <returns>New instance of <see cref="Component"/>, or existing instance if already on this entity</returns>
		public T AddComponent<T>() where T : Component => Entity?.AddComponent<T>() ?? null;

		/// <returns>Instance of <see cref="Component"/> attached to entity, or null if not found</returns>
		public T GetComponent<T>() where T : Component => Entity?.GetComponent<T>() ?? null;

		/// <summary>
		/// Removes a <see cref="Component"/> from this entity
		/// </summary>
		/// <returns>Success state of removal</returns>
		public bool RemoveComponent<T>() where T : Component => Entity?.RemoveComponent<T>() ?? false;
		#endregion

		#region Internal Calls
		// Called from C++
		internal void aqua_Initialise(ulong worldID, ulong entityID) => Entity = new Entity(World.Get(worldID), entityID);
		#endregion
	}
}
