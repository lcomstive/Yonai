namespace AquaEngine
{
	public abstract class Component
	{
		public World World => Entity.World;
		public Entity Entity { get; private set; }

		public Component() => Log.Debug("Component constructor");

		public void Initialise(uint worldID, uint entityID)
		{
			Log.Debug($"Initialised component [{worldID}.{entityID}]");
			Entity = new Entity(World.Get(worldID), entityID);
		}
	}
}
