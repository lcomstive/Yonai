using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Resource
	{
		private static Dictionary<UUID, ResourceBase> s_Instances = new Dictionary<UUID, ResourceBase>();

		/// <summary>
		/// Retrieves the ID of resource at <paramref name="path"/>
		/// </summary>
		/// <returns>ID of found resource, or <code>uint.MaxValue</code> if not found</returns>
		public static UUID GetID(string path) => _GetID(path);

		/// <summary>
		/// Gets the path associated with the resource matching ID <paramref name="resourceID"/>
		/// <returns>Path of loaded resource, or empty if not valid</returns>
		public static string GetPath(UUID resourceID) => _GetPath(resourceID);

		/// <summary>
		/// Creates a copy of resource at <paramref name="resourceID"/>, with the given path
		/// </summary>
		/// <returns>ID of the newly created resource</returns>
		public static UUID Duplicate(UUID resourceID, string newPath)
		{
			UUID newID = _Duplicate(resourceID, newPath);
			if(newID == UUID.Invalid)
				return newID;
			ResourceBase resource = s_Instances[resourceID].Clone(newID, newPath);
			resource._Load();
			s_Instances.Add(newID, resource);
			return newID;
		}

		public static T Duplicate<T>(UUID resourceID, string newPath) where T : ResourceBase, new()
		{
			UUID newID = Duplicate(resourceID, newPath);
			if(newID == UUID.Invalid)
				return null;
			return (T)s_Instances[resourceID];
		}

		/// <returns>True if resource is found matching <paramref name="resourceID"/></returns>
		public static bool Exists(UUID resourceID) => _Exists(resourceID);

		/// <summary>
		/// Loads a new resource with virtual path <paramref name="path"/>.
		/// If resource at virtual path already exists, returns existing resource.
		/// When resource is not found at virtual path, creates new one with given args.
		/// </summary>
		/// <param name="path">Virtual path to give loaded resource. This is a name used only for loading resources and does not represent a filepath.</param>
		/// <param name="args">Arguments to pass to resource during creation</param>
		/// <returns>Found or created instance of resource</returns>
		public static T Load<T>(string path, IImportSettings importSettings) where T : ResourceBase, new()
		{
			// Check for existing cached instance 
			UUID resourceID = _GetID(path);
			if (resourceID != UUID.Invalid && s_Instances.ContainsKey(resourceID))
				return (T)s_Instances[resourceID];
			
			T instance = new T();

			// Check if native instance exists
			if(resourceID == UUID.Invalid)
			{
				// No resource found in unmanaged code, create new resource
				instance.ResourceID = resourceID;
				instance.ResourcePath = path;
				instance.ImportSettings = importSettings;

				instance._Load();
				instance.Import(importSettings);

				// Cache created resource
				s_Instances.Add(instance.ResourceID, instance);
			}
			else
				// Add found unmanaged resource, cache in s_Instances
				LoadExistingResource<T>(instance, resourceID);

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
				s_Instances.Remove(resourceID);
			}

			_Unload(resourceID);
		}

		/// <returns>Instance of resource with matching ID, or null if ID is invalid</returns>
		public static T Get<T>(UUID resourceID) where T : ResourceBase, new()
		{
			// Check for valid ID
			if (resourceID == UUID.Invalid || !Exists(resourceID))
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

			// Create new instance
			T instance = new T();
			LoadExistingResource<T>(instance, resourceID);
			return instance;
		}

		public static T Get<T>(string path) where T : ResourceBase, new()
		{
			UUID resourceID = GetID(path);
			return resourceID == UUID.Invalid ? null : Get<T>(resourceID);
		}

		public static void SaveDatabase()
		{
			// TODO: Save all resources with ISerializable interface to file (e.g. "editor://Resources.json")
		}

		public static void LoadDatabase()
		{
			// TODO: Load resources with ISerializable interface from file
		}

		private static void LoadExistingResource<T>(T instance, UUID resourceID) where T : ResourceBase, new()
		{
			instance.ResourceID = resourceID;

			// Check if resource is a native (C++) interop resource
			NativeResourceBase nativeResource = instance as NativeResourceBase;
			if(nativeResource != null)
				nativeResource?.SetHandle(_GetInstance(resourceID));

			s_Instances.Add(resourceID, instance);

			instance._Load();
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetID(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unload(ulong resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Exists(ulong resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _Duplicate(ulong resourceID, string newPath);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPath(ulong resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetInstance(ulong resourceID);
		#endregion
	}
}