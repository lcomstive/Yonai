namespace AquaEngine
{
	public class Quaternion
	{
		public float x, y, z, w;

		public static Quaternion Identity => new Quaternion(0, 0, 0, 1);

		public Quaternion(float x, float y, float z, float w)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
		}
	}
}
