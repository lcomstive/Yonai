using System;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("[{ResourceID}]")]
	public abstract class ResourceBase : ICloneable
	{
		public uint ResourceID { get; internal set; }

		internal abstract bool Load(string path, params object[] args);

		internal virtual void OnLoad() { }
		internal virtual void OnUnload() { }
		internal virtual void OnDuplicated(ResourceBase original) { }

		public virtual object Clone()
		{
			ResourceBase clone = (ResourceBase)this.MemberwiseClone();
			clone.OnDuplicated(this);
			return clone;
		}

		public static implicit operator uint(ResourceBase resourceBase) => resourceBase.ResourceID;
	}

	public abstract class NativeResourceBase : ResourceBase
	{
		protected IntPtr Handle { get; set; }

		internal void LoadFromHandle(IntPtr handle) => Handle = handle;

		public override object Clone()
		{
			NativeResourceBase clone = (NativeResourceBase)base.Clone();
			clone.Handle = Handle;
			return clone;
		}
	}
}