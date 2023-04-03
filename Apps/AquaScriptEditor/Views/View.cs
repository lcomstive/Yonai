using System;
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

		internal void _Open() { try { Opened(); } catch (Exception e) { Log.Exception(e); } }
		internal void _Close() { try { Closed(); } catch (Exception e) { Log.Exception(e); } }
		internal void _Draw() { try { Draw(); } catch (Exception e) { Log.Exception(e); } }
		internal void _Update() { try { Update(); } catch (Exception e) { Log.Exception(e); } }
	}
}