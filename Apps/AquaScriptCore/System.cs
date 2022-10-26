using System;

namespace AquaEngine
{
	public abstract class System
	{
		public World World { get; private set; }

		#region Unmanaged Calls
		private void Initialise(uint worldID) => World = World.Get(worldID);

		internal virtual void _Draw() { }
		internal virtual void _Start() { }
		internal virtual void _Update() { }
		internal virtual void _Destroy() { }
		internal virtual void _Enable(bool enable) { }
		#endregion
	}
}

