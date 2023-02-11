using System;

namespace AquaEngine
{
	public abstract class ResourceBase
	{
		public uint ResourceID { get; internal set; }

		internal abstract bool Load(string path, params object[] args);

		internal virtual void OnLoad() { }
		internal virtual void OnUnload() { }

		public static implicit operator uint(ResourceBase resourceBase) => resourceBase.ResourceID;
	}

	public abstract class NativeResourceBase : ResourceBase
	{
		protected IntPtr Handle { get; set; }

		internal void LoadFromHandle(IntPtr handle) => Handle = handle;
	}
}