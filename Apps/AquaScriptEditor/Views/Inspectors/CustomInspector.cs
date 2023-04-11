
namespace AquaEditor
{
	public abstract class CustomInspector
	{
		public object Target { get; internal set; }

		/// <summary>
		/// Called when this inspector is opened
		/// </summary>
		public virtual void Opened() { }

		/// <summary>
		/// Called when this inspector is closed
		/// </summary>
		public virtual void Closed() { }

		/// <summary>
		/// Called whenever <see cref="Target"/> gets set
		/// </summary>
		public virtual void OnTargetChanged() { }

		/// <summary>
		/// Called once per frame
		/// </summary>
		public abstract void DrawInspector();
	}
}