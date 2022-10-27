using System;

namespace AquaEngine
{
	public abstract class Component
	{
		/// <summary>
		/// The <see cref="World"/> this component exists within.
		/// This matches the <see cref="World"/> of the attached <see cref="Entity"/>.
		/// </summary>
		public World World => Entity.World;

		/// <summary>
		/// <see cref="Entity"/> this component is attached to.
		/// </summary>
		public Entity Entity { get; private set; }

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
		// Called from C++
		private void aqua_Initialise(uint worldID, uint entityID) => Entity = new Entity(World.Get(worldID), entityID);

		// Called from C++
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
