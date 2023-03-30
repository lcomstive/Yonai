
namespace AquaEditor
{
	public abstract class CustomInspector
	{
		/// <summary>
		/// Called when this inspector is opened
		/// </summary>
		public virtual void Opened() { }

		/// <summary>
		/// Called when this inspector is closed
		/// </summary>
		public virtual void Closed() { }

		/// <summary>
		/// Called once per frame
		/// </summary>
		public abstract void DrawInspector(object target);
	}
}