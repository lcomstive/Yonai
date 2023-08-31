using System.Runtime.CompilerServices;

namespace Yonai
{
	public static class Input
	{
		public static bool Enabled
		{
			get => _InputEnabled();
			set => _InputEnable(value);
		}

		#region Keys
		/// <returns>True if the key is not pressed down</returns>
		public static bool IsKeyUp(Key key)			=> _IsKeyUp((int)key);

		/// <returns>True if the key is currently pressed down</returns>
		public static bool IsKeyDown(Key key)		=> _IsKeyDown((int)key);

		/// <returns>True if the key is was pressed down this frame</returns>
		public static bool IsKeyPressed(Key key)	=> _IsKeyPressed((int)key);

		/// <returns>True if the key is was released this frame</returns>
		public static bool IsKeyReleased(Key key)	=> _IsKeyReleased((int)key);
		#endregion

		#region Mouse
		/// <summary>
		/// Amount the scroll wheel has changed since the previous frame
		/// </summary>
		public static float ScrollDelta => _ScrollDelta();

		/// <summary>
		/// Current position of the mouse, in screen pixels relative to the top-left corner of the window
		/// </summary>
		public static Vector2 MousePosition
		{
			get
			{
				_GetMousePosition(out Vector2 value);
				return value;
			}
			set => _SetMousePosition(value);
		}

		/// <summary>
		/// Amount the mouse has moved, in pixels, since the previous frame
		/// </summary>
		public static Vector2 MouseDelta
		{
			get
			{
				_GetMouseDelta(out Vector2 value);
				return value;
			}
		}

		/// <summary>
		/// Sets the mouse visibility & disabled state
		/// </summary>
		public static MouseState MouseState
		{
			get => (MouseState)_GetMouseState();
			set => _SetMouseState((int)value);
		}

		/// <summary>
		/// Checks if the mouse is showing. This can be changed using <see cref="MouseState"/>
		/// </summary>
		public static bool IsMouseShowing => _IsMouseShowing();

		/// <returns>True if the button is not pressed down</returns>
		public static bool IsMouseUp(MouseButton button) => _IsMouseUp((int)button);

		/// <returns>True if the button is currently pressed down</returns>
		public static bool IsMouseDown(MouseButton button) => _IsMouseDown((int)button);

		/// <returns>True if the button is was pressed down this frame</returns>
		public static bool IsMousePressed(MouseButton button) => _IsMousePressed((int)button);

		/// <returns>True if the button is was released this frame</returns>
		public static bool IsMouseReleased(MouseButton button) => _IsMouseReleased((int)button);
		#endregion

		#region Gamepad
		/// <returns>True if a gamepad with index is connected & valid</returns>
		public static bool IsGamepadConnected(int gamepadIndex) => _IsGamepadConnected(gamepadIndex);

		/// <returns>Value of axis on gamepad at <paramref name="gamepadIndex"/>, or 0 if not connected</returns>
		public static float GetAxis(int gamepadIndex, GamepadAxis axis) => _GetAxis(gamepadIndex, (int)axis);

		/// <returns>True if button is not pressed on gamepad at <paramref name="gamepadIndex"/></returns>
		public static bool IsButtonUp(int gamepadIndex, GamepadButton button) => _IsButtonUp(gamepadIndex, (int)button);

		/// <returns>True if button is pressed on gamepad at <paramref name="gamepadIndex"/></returns>
		public static bool IsButtonDown(int gamepadIndex, GamepadButton button) => _IsButtonDown(gamepadIndex, (int)button);
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _InputEnabled();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _InputEnable(bool enable);

		// Keys
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsKeyUp(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsKeyDown(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsKeyPressed(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsKeyReleased(int key);

		// Mouse
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern float _ScrollDelta();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _GetMousePosition(out Vector2 output);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _SetMousePosition(Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _GetMouseDelta(out Vector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _IsMouseUp(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _IsMouseDown(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _IsMousePressed(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _IsMouseReleased(int key);

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern int _GetMouseState();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _IsMouseShowing();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _SetMouseState(int state);


		// Gamepad
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsGamepadConnected(int gamepadIndex);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetAxis(int gamepadIndex, int axis);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsButtonUp(int gamepadIndex, int button);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsButtonDown(int gamepadIndex, int button);
		#endregion
	}
}
