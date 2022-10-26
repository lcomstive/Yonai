namespace AquaEngine
{
	public abstract class Component
	{
		public World World => Entity.World;
		public Entity Entity { get; private set; }


		protected virtual void Start() { }
		protected virtual void Update() { }
		protected virtual void OnEnabled() { }
		protected virtual void OnDisabled() { }
		protected virtual void OnDestroyed() { }

		#region Unmanaged Calls
		private void Initialise(uint worldID, uint entityID) => Entity = new Entity(World.Get(worldID), entityID);

		internal virtual void _Start() => Start();
		internal virtual void _Update() => Update();
		internal virtual void _Destroy() => OnDestroyed();

		internal virtual void _Enable(bool enable)
		{
			if (enable)
				OnEnabled();
			else
				OnDisabled();
		}
		#endregion
	}
}
