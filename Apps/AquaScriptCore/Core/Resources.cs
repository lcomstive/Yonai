using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Resource
	{
		private static Dictionary<uint, ResourceBase> s_Instances = new Dictionary<uint, ResourceBase>();

		/// <summary>
		/// Retrieves the ID of resource at <paramref name="path"/>
		/// </summary>
		/// <returns>ID of found resource, or <code>uint.MaxValue</code> if not found</returns>
		public static uint GetID(string path) => _GetID(path);

		/// <summary>
		/// Gets the path associated with the resource matching ID <paramref name="resourceID"/>
		/// <returns>Path of loaded resource, or empty if not valid</returns>
		public static string GetPath(uint resourceID) => _GetPath(resourceID);

		/// <summary>
		/// Creates a copy of resource at <paramref name="resourceID"/>, with the given path
		/// </summary>
		/// <returns>ID of the newly created resource</returns>
		public static uint Duplicate(uint resourceID, string newPath)
		{
			uint newID = _Duplicate(resourceID, newPath);
			if(newID == uint.MaxValue)
				return newID;
			s_Instances.Add(newID, (ResourceBase)s_Instances[resourceID].Clone());
			return newID;
		}

		public static T Duplicate<T>(uint resourceID, string newPath) where T : ResourceBase, new()
		{
			uint newID = Duplicate(resourceID, newPath);
			if(newID == uint.MaxValue)
				return null;
			return (T)s_Instances[resourceID];
		}

		/// <returns>True if resource is found matching <paramref name="resourceID"/></returns>
		public static bool Exists(uint resourceID) => _Exists(resourceID);

		/// <summary>
		/// Loads a new resource with virtual path <paramref name="path"/>.
		/// If resource at virtual path already exists, returns existing resource.
		/// When resource is not found at virtual path, creates new one with given args.
		/// </summary>
		/// <param name="path">Virtual path to give loaded resource. This is a name used only for loading resources and does not represent a filepath.</param>
		/// <param name="args">Arguments to pass to resource during creation</param>
		/// <returns>Found or created instance of resource</returns>
		public static T Load<T>(string path, params object[] args) where T : ResourceBase, new()
		{
			// Check for existing cached instance 
			uint resourceID = _GetID(path);
			if (resourceID != uint.MaxValue && s_Instances.ContainsKey(resourceID))
				return (T)s_Instances[resourceID];
			
			T instance = new T();

			// Check if native instance exists
			if(resourceID == uint.MaxValue)
			{
				// No resource found in unmanaged code,
				// create new resource with args
				instance.ResourceID = resourceID;
				instance.Load(path, args);
				instance.OnLoad();

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
		public static void Unload(uint resourceID)
		{
			if(s_Instances.ContainsKey(resourceID))
			{
				s_Instances[resourceID].OnUnload();
				s_Instances.Remove(resourceID);
			}

			_Unload(resourceID);
		}

		/// <returns>Instance of resource with matching ID, or null if invalid ID is provided</returns>
		public static T Get<T>(uint resourceID) where T : ResourceBase, new()
		{
			// Check for valid ID
			if (resourceID == uint.MaxValue || !Exists(resourceID))
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
			uint resourceID = GetID(path);
			return resourceID == uint.MaxValue ? null : Get<T>(resourceID);
		}

		private static void LoadExistingResource<T>(T instance, uint resourceID) where T : ResourceBase, new()
		{
			instance.ResourceID = resourceID;

			// Check if resource is a native (C++) interop resource
			NativeResourceBase nativeResource = instance as NativeResourceBase;
			if(nativeResource != null)
				nativeResource?.LoadFromHandle(_GetInstance(resourceID));

			s_Instances.Add(resourceID, instance);

			instance.OnLoad();
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetID(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Unload(uint resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _Exists(uint resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _Duplicate(uint resourceID, string newPath);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetPath(uint resourceID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetInstance(uint resourceID);
		#endregion
	}
}