using AquaEngine;

namespace AquaEditor.Views
{
	public abstract class View
	{
		/// <summary>
		/// Called once per frame
		/// </summary>
		protected virtual void Update() { }

		/// <summary>
		/// Used to draw to the screen. Typically ImGUI calls.
		/// </summary>
		protected virtual void Draw() { }

		internal void _Draw() => Draw();
		internal void _Update() => Update();
	}
}