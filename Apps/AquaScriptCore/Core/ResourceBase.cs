using System;

namespace AquaEngine
{
	public abstract class ResourceBase
	{
		public uint ResourceID { get; protected set; }
		protected IntPtr Handle { get; set; }

		internal abstract bool Load(uint resourceID, IntPtr handle);
		internal abstract bool Load(string path, params object[] args);

		internal virtual void OnUnload() { }
	}
}