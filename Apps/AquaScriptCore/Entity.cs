namespace AquaEngine
{
	public class Entity
	{
		public uint ID { get; internal set; }
		public World World { get; internal set; }

		public const uint InvalidEntityID = uint.MaxValue;

		internal Entity(World world, uint id)
		{
			World = world;
			ID = id;
		}
	}
}
