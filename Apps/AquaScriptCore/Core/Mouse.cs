using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Mouse
	{
		/// <summary>
		/// Amount the scroll wheel has changed since the previous frame
		/// </summary>
		public static float ScrollDelta => Input._aqua_internal_ScrollDelta();

		/// <summary>
		/// Current position of the mouse, in screen pixels relative to the top-left corner of the window
		/// </summary>
		public static Vector2 Position
		{
			get
			{
				Input._aqua_internal_GetMousePosition(out Vector2 value);
				return value;
			}
			set => Input._aqua_internal_SetMousePosition(value);
		}

		/// <summary>
		/// Amount the mouse has moved, in pixels, since the previous frame
		/// </summary>
		public static Vector2 DeltaPosition
		{
			get
			{
				Input._aqua_internal_GetMouseDelta(out Vector2 value);
				return value;
			}
		}

		/// <summary>
		/// Sets the mouse visibility & disabled state
		/// </summary>
		public static MouseState State
		{
			get => (MouseState)Input._aqua_internal_GetMouseState();
			set => Input._aqua_internal_SetMouseState((int)value);
		}

		/// <summary>
		/// Checks if the mouse is showing. This can be changed using <see cref="MouseState"/>
		/// </summary>
		public static bool Showing => Input._aqua_internal_IsMouseShowing();

		/// <returns>True if the button is not pressed down</returns>
		public static bool IsButtonUp(MouseButton button) => Input._aqua_internal_IsMouseUp((int)button);

		/// <returns>True if the button is currently pressed down</returns>
		public static bool IsButtonDown(MouseButton button) => Input._aqua_internal_IsMouseDown((int)button);

		/// <returns>True if the button is was pressed down this frame</returns>
		public static bool IsButtonPressed(MouseButton button) => Input._aqua_internal_IsMousePressed((int)button);

		/// <returns>True if the button is was released this frame</returns>
		public static bool IsButtonReleased(MouseButton button) => Input._aqua_internal_IsMouseReleased((int)button);
	}
}
