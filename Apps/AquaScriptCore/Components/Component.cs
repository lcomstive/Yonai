namespace AquaEngine
{
	public abstract class Component
	{
		public World World => Entity.World;
		public Entity Entity { get; private set; }

		public Component() => Log.Debug("Component constructor");

		private void Initialise(uint worldID, uint entityID) => Entity = new Entity(World.Get(worldID), entityID);

		protected virtual void Start() { }
		protected virtual void Update() { }
		protected virtual void OnDestroy() { }
	}
}
