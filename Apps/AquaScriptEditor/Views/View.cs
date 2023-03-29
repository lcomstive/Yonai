using AquaEngine;

namespace AquaEditor.Views
{
	public abstract class View
	{
		/// <summary>
		/// Called when opened
		/// </summary>
		protected virtual void Opened() { }

		/// <summary>
		/// Called when about to be closed
		/// </summary>
		protected virtual void Closed() { }

		/// <summary>
		/// Called once per frame
		/// </summary>
		protected virtual void Update() { }

		/// <summary>
		/// Used to draw to the screen. Typically ImGUI calls.
		/// </summary>
		protected virtual void Draw() { }

		internal void _Open() => Opened();
		internal void _Close() => Closed();
		internal void _Draw() => Draw();
		internal void _Update() => Update();
	}
}