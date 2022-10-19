namespace AquaEngine
{
	public abstract class Component
	{
		public World World => Entity.World;
		public Entity Entity { get; internal set; }

		public Component(Entity entity) => Entity = entity;
	}
}
