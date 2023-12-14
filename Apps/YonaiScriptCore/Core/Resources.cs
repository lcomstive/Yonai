using System;
using Yonai.IO;
using Yonai.Graphics;
using Newtonsoft.Json;
using System.Reflection;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Yonai.Graphics.Backends.Vulkan;

namespace Yonai
{
	public static class Resource
	{
		private const string DatabaseFilePath = "project://Resources.json";
		private static Dictionary<string, UUID> s_Paths = new Dictionary<string, UUID>();
		private static Dictionary<UUID, ResourceBase> s_Instances = new Dictionary<UUID, ResourceBase>();

		/// <summary>
		/// Retrieves the ID of resource at <paramref name="path"/>
		/// </summary>
		/// <returns>ID of found resource, or <see cref="UUID.Invalid"/> if not found</returns>
		public static UUID GetID(string path)
		{
			// Check native resourceIDs
			UUID id = _GetID(path);
			if(id != UUID.Invalid) return id;
			if(s_Paths.ContainsKey(path))
				return s_Paths[path];
			return UUID.Invalid;
		}

		/// <summary>
		/// Gets the path associated with the resource matching ID <paramref name="resourceID"/>
		/// <returns>Path of loaded resource, or null if not valid</returns>
		public static string GetPath(UUID resourceID) =>
			_GetPath(resourceID) ?? (Exists(resourceID) ? s_Instances[resourceID].ResourcePath : null);

		/// <returns>True if resource is found matching <paramref name="resourceID"/></returns>
		public static bool Exists(UUID resourceID) => s_Instances.ContainsKey(resourceID) || _Exists(resourceID);

		/// <returns>True if resource path is found matching <paramref name="path"/></returns>
		public static bool Exists(string path) => s_Paths.ContainsKey(path) && s_Instances.ContainsKey(s_Paths[path]);
		public static bool Exists(VFSFile file) => Exists(file.FullPath);

		/// <summary>
		/// Loads a new resource with virtual path <paramref name="path"/>.
		/// If resource at virtual path already exists, returns existing resource.
		/// When resource is not found at virtual path, creates new one with given args.
		/// </summary>
		/// <param name="path">Asset filepath, usable from VFS</param>
		/// <param name="importSettings">Arguments to pass to resource during creation</param>
		/// <param name="saveToDisk">When false, does not save loaded file to disk immediately after creation</param>
		/// <returns>Found or created instance of resource</returns>
		public static T Load<T>(string path, IImportSettings importSettings = null, bool saveToDisk = true) where T : ResourceBase, new()
		{
			// Check for existing cached instance 
			UUID resourceID = GetID(path);
			if(resourceID != UUID.Invalid && s_Instances.ContainsKey(resourceID))
				return (T)s_Instances[resourceID];
			if (Exists(path))
				return Get<T>(path);

			T instance = new T();
			instance.ResourcePath = path;
			instance.ResourceID = resourceID;

			Log.Trace($"Loading '{path}' ({typeof(T).Name})");

			// Check if native instance exists
			if(resourceID == UUID.Invalid)
			{
				// No resource found in unmanaged code, create new resource
				instance.ImportSettings = importSettings;

				instance._Load();
				if(instance.ResourceID == UUID.Invalid)
					instance.ResourceID = _CreateID(); 
				
				instance.Import(importSettings);

				// Cache created resource
				s_Instances.Add(instance.ResourceID, instance);
				s_Paths.Add(path, instance.ResourceID);
			}
			else
			{
				// Add found unmanaged resource, cache in s_Instances
				LoadExistingUnmanagedResource(instance, resourceID);

				if(!s_Paths.ContainsKey(path))
					s_Paths.Add(path, resourceID);
			}

			if(saveToDisk)
				SaveToDisk(instance, true);

			return instance;
		}


		/// <summary>
		/// Loads a new resource with virtual path <paramref name="path"/>.
		/// If resource at virtual path already exists, returns existing resource.
		/// When resource is not found at virtual path, creates new one with given args.
		/// </summary>
		/// <param name="path">Asset filepath, usable from VFS</param>
		/// <param name="saveToDisk">When false, does not save loaded file to disk immediately after creation</param>
		/// <returns>Found or created instance of resource</returns>
		public static T Load<T>(string path, bool saveToDisk) where T : ResourceBase, new()
			=> Load<T>(path, null, saveToDisk);


		public static ResourceBase LoadFromUUID(UUID resourceID, string path, Type type)
		{
			// Ensure type inherits from ResourceBase
			if(!typeof(ResourceBase).IsAssignableFrom(type))
				throw new ArgumentException($"Resources.Load() type '{type?.Name ?? "<null>"}' is invalid");

			// Check for existing cached instance 
			if(resourceID != UUID.Invalid && s_Instances.ContainsKey(resourceID))
				return s_Instances[resourceID];
			if (s_Paths.ContainsKey(path))
				return s_Instances[s_Paths[path]];

			ResourceBase instance = (ResourceBase)Activator.CreateInstance(type);
			instance.ResourcePath = path;
			instance.ResourceID = resourceID;

			s_Paths.Add(path, instance.ResourceID);

			// Check if native instance exists
			if(resourceID == UUID.Invalid)
			{
				// No resource found in unmanaged code, create new resource
				instance._Load();

				// Cache created resource
				s_Instances.Add(instance.ResourceID, instance);

				if (instance is ISerializable && !LoadFromDisk(instance))
					Log.Warning($"Failed to deserialize resource from disk [{resourceID}] '{path}'");
			}
			else
				// Add found unmanaged resource, cache in s_Instances
				LoadExistingUnmanagedResource(instance, resourceID);
			return instance;
		}

		/// <summary>
		/// Removes a resource from memory.
		/// </summary>
		public static void Unload(UUID resourceID)
		{
			if(s_Instances.ContainsKey(resourceID))
			{
				s_Instances[resourceID]._Unload();
				Log.Trace($"Unloaded '{s_Instances[resourceID].ResourcePath}'");
				s_Instances.Remove(resourceID);
			}

			foreach(var pair in s_Paths)
			{
				if(pair.Value != resourceID)
					continue;
				s_Paths.Remove(pair.Key);
				break;
			}

			_Unload(resourceID);
		}

		/// <returns>Instance of resource with matching ID, or null if ID is invalid</returns>
		public static T Get<T>(UUID resourceID, bool createIfNotExist = true) where T : ResourceBase, new()
		{
			// Check for valid ID
			if(resourceID == UUID.Invalid || !Exists(resourceID))
				return null;

			// Check for cached instance
			if(s_Instances.ContainsKey(resourceID))
			{
				object foundInstance = s_Instances[resourceID];
				if(foundInstance is T)
					return (T)foundInstance;
				else
				{
					Log.Error($"Tried to load resource of type '{typeof(T).Name}', but expected '{foundInstance.GetType().Name}'");
					return null;
				}
			}

			if (!createIfNotExist)
				return null;

			// Create new instance
			T instance = new T();
			instance.ResourceID = resourceID;
			instance.ResourcePath = _GetPath(resourceID);
			Log.Trace($"Get<{typeof(T).Name}>({resourceID}) not cached, trying a native resource at '{instance.ResourcePath}'");
			LoadExistingUnmanagedResource(instance, resourceID);
			return instance;
		}
		
		public static ResourceBase Get(UUID resourceID)
		{
			// Check for cached instance
			if (s_Instances.ContainsKey(resourceID))
				return s_Instances[resourceID];
			else
				return null;
		}

		/// <summary>
		/// Gets the type of a resource, or null if not found
		/// </summary>
		public static Type GetType(UUID resourceID) => Exists(resourceID) ? s_Instances[resourceID].GetType() : null;

		public static T Get<T>(string path) where T : ResourceBase, new()
		{
			UUID resourceID = GetID(path);
			return resourceID == UUID.Invalid ? null : Get<T>(resourceID);
		}

		/// <summary>
		/// Saves all resources to a file
		/// </summary>
		public static void SaveDatabase(string database = DatabaseFilePath)
		{
			JArray resources = new JArray();
			foreach(var pair in s_Instances)
			{
				// Check if should serialize
				if (!pair.Value.GetType().GetCustomAttribute<SerializeAttribute>()?.ShouldSerialize ?? false)
					continue;

				JObject resource = new JObject();
				resource["ID"] = pair.Key.ToString();
				resource["Path"] = pair.Value.ResourcePath;

				Type resourceType = pair.Value.GetType();
				resource["Type"] = resourceType.FullName;
				if (resourceType.Namespace != "Yonai")
					resource["Type"] += ", " + resourceType.Assembly.GetName().Name;

				resources.Add(resource);
			}

			Log.Debug($"Saving resources to '{database}' ({VFS.ExpandPath(database)?.FullPath ?? string.Empty})");

			string json = JsonConvert.SerializeObject(resources, Formatting.Indented);
			VFS.Write(database, json.Replace("\r\n", "\n"));
		}

		/// <summary>
		/// Loads resources from a file
		/// </summary>
		public static void LoadDatabase(string database = DatabaseFilePath)
		{
			// Check that file exists
			if (!VFS.Exists(database))
			{
				// Database does not exist, create it
				Mesh.LoadPrimitives();
				CreateDefaultResources();
				SaveDatabase();
				return;
			}

			Log.Debug($"Loading resources from '{database}' ({VFS.ExpandPath(database)?.FullPath ?? string.Empty})");

			JArray root = JsonConvert.DeserializeObject<JArray>(VFS.ReadText(database));
			foreach(JObject resource in root)
			{
				UUID id = (UUID)ulong.Parse(resource["ID"].Value<string>());
				string resourcePath = resource["Path"].Value<string>();

				if (s_Instances.ContainsKey(id))
					continue; // Already loaded in managed memory

				string typeName = resource["Type"].Value<string>();
				Type type = Type.GetType(typeName);

				ResourceBase instance = LoadFromUUID(id, resourcePath, type);
				if(instance is ISerializable && !LoadFromDisk(instance))
					Unload(id); // If failed to load from disk, *most* likely will be in a call to Load with the correct import settings
			}

			CreateDefaultResources();
			Mesh.LoadPrimitives();
		}

		private static void LoadExistingUnmanagedResource(ResourceBase instance, UUID resourceID)
		{
			// Check if resource is a native (C++) interop resource
			NativeResourceBase nativeResource = instance as NativeResourceBase;

			if (nativeResource)
			{
				nativeResource.ResourceID = resourceID;
				nativeResource.SetHandle(_GetInstance(resourceID));
				nativeResource._OnNativeLoad();
			}

			instance._Load();

			s_Instances.Add(resourceID, instance);
		}

		private static void CreateDefaultResources()
		{
			// Textures //
			Load<Texture>("assets://Textures/Grid.png");

			// Materials //
			Load<Material>("assets://Materials/Default3D.material", new MaterialImportSettings
			{
				Shader = Load<Shader>("assets://Shaders/Default3D.shader", new ShaderImportSettings
				{
					StagePaths = new Dictionary<VkShaderStage, VFSFile>()
					{
						{ VkShaderStage.Vertex, "assets://Shaders/Vulkan/vert.spv" },
						{ VkShaderStage.Fragment, "assets://Shaders/Vulkan/frag.spv" },
					}
				})
			});
			Load<Material>("assets://Materials/Default2D.material", new MaterialImportSettings
			{
				Shader = Load<Shader>("assets://Shaders/Default2D.shader", new ShaderImportSettings
				{
					StagePaths = new Dictionary<VkShaderStage, VFSFile>()
					{
						{ VkShaderStage.Vertex, "assets://Shaders/Sprite.vert" },
						{ VkShaderStage.Fragment, "assets://Shaders/Sprite.frag" },
					}
				})
			});
		}

		public static void SaveToDisk(ResourceBase resource, bool suppressWarnings = false)
		{
			if (resource == null) return;

			ISerializable serializable = resource as ISerializable;
			if(serializable == null)
			{
				if(!suppressWarnings)
					Log.Warning($"Failed to serialize '{resource.ResourcePath}' - Not a serializable type");
				return;
			}

			SerializeFileOptionsAttribute serializeFileOptions = resource.GetType().GetCustomAttribute<SerializeFileOptionsAttribute>();
			string path = resource.ResourcePath;
			if (serializeFileOptions != null)
				path += ".cache";

			Log.Trace($"Saving resource '{resource.ResourcePath}' " + (serializeFileOptions == null ? string.Empty : "[cache]"));

			VFSFile vfsFile = resource.ResourcePath;
			VFSMapping mapping = VFS.GetMapping(resource.ResourcePath, false, FilePermissions.Write);
			if (!mapping.Exists(vfsFile.ParentDirectory))
			{
				Log.Debug($"Creating directory '{vfsFile.ParentDirectory}'");
				mapping.CreateDirectory(vfsFile.ParentDirectory);
			}

			if (mapping)
				mapping.Write(path, serializable);
			else
				Log.Warning($"Failed to find writable mapping for '{resource.ResourcePath}'");
		}

		public static bool LoadFromDisk(ResourceBase resource, bool suppressWarnings = false)
		{
			if(resource == null) return false;

			ISerializable serializable = resource as ISerializable;
			if (serializable == null)
			{
				if (!suppressWarnings)
					Log.Warning($"Failed to serialize '{resource.ResourcePath}' - Not a serializable type");
				return false;
			}

			SerializeFileOptionsAttribute serializeFileOptions = resource.GetType().GetCustomAttribute<SerializeFileOptionsAttribute>();
			string path = resource.ResourcePath;
			if (serializeFileOptions != null)
				path += ".cache";

			if (!VFS.Exists(path))
			{
				if (!suppressWarnings)
					Log.Warning($"Failed to deserialize '{resource.ResourcePath}' " +
						(serializeFileOptions == null ? string.Empty : "[cache]")  + " - VFS could not get path");
				return false;
			}

			Log.Trace($"Loading resource '{path}' from disk...");
			resource._Load();

			VFSMapping mapping = VFS.GetMapping(path);
			serializable.OnDeserialize(mapping.ReadJSON(path));
			return true;
		}

        /// <summary>
        /// Presets an ID for a specified path, without loading the resource.
        /// When the resource has been loaded in using <see cref="Load{T}(string, IImportSettings, bool)"/>,
        /// it will use the provided ID instead of generating a random one.
        /// This is overriden when loading from resource database.
        /// </summary>
        public static void SetIDForPath(string resourcePath, UUID resourceID)
        {
			if (!s_Paths.ContainsKey(resourcePath))
			{
				s_Paths.Add(resourcePath, resourceID);
				Log.Debug($"Preset for '{resourcePath}' [{resourceID}]");
			}
        }

        #region Internal Calls
        [MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _CreateID();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetID(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unload(ulong resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Exists(ulong resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _Duplicate(ulong resourceID, string newPath);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPath(ulong resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetInstance(ulong resourceID);

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _Print();
		#endregion
	}
}