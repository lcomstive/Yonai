using System;

namespace AquaEngine
{
	public abstract class System
	{
		public World World { get; private set; }

		/// <summary>
		/// Called once per frame after <see cref="Update"/>,
		/// when the screen is being drawn to
		/// </summary>
		protected virtual void Draw() { }

		/// <summary>
		/// Called when this script is created and attached to an <see cref="Entity"/> (after <see cref="OnEnabled"/>)
		/// </summary>
		protected virtual void Start() { }

		/// <summary>
		/// Called once per frame
		/// </summary>
		protected virtual void Update() { }

		/// <summary>
		/// Called when script has been enabled.
		/// Can also be called when attached to an <see cref="Entity"/>,
		/// this function is called before <see cref="Start"/> in this case
		/// </summary>
		protected virtual void Enabled() { }

		/// <summary>
		/// Called when the script has been disabled.
		/// Can also be called when attached <see cref="Entity"/> is being destroyed,
		/// this function is called before <see cref="OnDestroyed"/> in this case
		/// </summary>
		protected virtual void Disabled() { }

		/// <summary>
		/// Called when the attached <see cref="Entity"/> is about to be destroyed (after <see cref="OnDisabled"/>)
		/// </summary>
		protected virtual void Destroyed() { }

		#region Unmanaged Calls
		private void aqua_Initialise(uint worldID) => World = World.Get(worldID);

		private void aqua_Enable(bool enable)
		{
			if (enable)
				Enabled();
			else
				Disabled();
		}
		#endregion
	}
}

