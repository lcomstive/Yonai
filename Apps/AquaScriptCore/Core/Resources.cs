using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Resource
	{
		private static Dictionary<uint, ResourceBase> s_Instances = new Dictionary<uint, ResourceBase>();

		public static uint GetID(string path) => _GetID(path);
		public static string GetPath(uint resourceID) => _GetPath(resourceID);
		public static uint Duplicate(uint resourceID, string newPath) => _Duplicate(resourceID, newPath);

		public static bool Exists(uint resourceID) => _Exists(resourceID);

		public static T Load<T>(string path, params object[] args) where T : ResourceBase, new()
		{
			// Check for existing cached instance 
			uint resourceID = _GetID(path);
			if (resourceID != uint.MaxValue && s_Instances.ContainsKey(resourceID))
				return (T)s_Instances[resourceID];
			
			T instance = new T();

			// Check if instance exists
			if(resourceID == uint.MaxValue)
			{
				// New resource, create from args
				instance.ResourceID = resourceID;
				instance.Load(path, args);
				instance.OnLoad();

				s_Instances.Add(instance.ResourceID, instance);
			}
			else
				// Add existing instance, cache in s_Instances
				LoadExistingResource<T>(instance, resourceID);

			return instance;
		}

		public static void Unload(uint resourceID)
		{
			if(s_Instances.ContainsKey(resourceID))
			{
				s_Instances[resourceID].OnUnload();
				s_Instances.Remove(resourceID);
			}

			_Unload(resourceID);
		}

		public static T Get<T>(uint resourceID) where T : ResourceBase, new()
		{
			// Check for valid ID
			if (resourceID == uint.MaxValue || !Exists(resourceID))
				return null;

			// Check for cached instance
			if (s_Instances.ContainsKey(resourceID))
				return (T)s_Instances[resourceID];

			// Create new instance
			T instance = new T();
			LoadExistingResource<T>(instance, resourceID);

			return instance;
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