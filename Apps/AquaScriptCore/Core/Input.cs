using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class Input
	{
		public static bool Enabled
		{
			get => _aqua_internal_InputEnabled();
			set => _aqua_internal_InputEnable(value);
		}

		#region Keys
		/// <returns>True if the key is not pressed down</returns>
		public static bool IsKeyUp(Key key)			=> _aqua_internal_IsKeyUp((int)key);

		/// <returns>True if the key is currently pressed down</returns>
		public static bool IsKeyDown(Key key)		=> _aqua_internal_IsKeyDown((int)key);

		/// <returns>True if the key is was pressed down this frame</returns>
		public static bool IsKeyPressed(Key key)	=> _aqua_internal_IsKeyPressed((int)key);

		/// <returns>True if the key is was released this frame</returns>
		public static bool IsKeyReleased(Key key)	=> _aqua_internal_IsKeyReleased((int)key);
		#endregion

		#region Mouse
		/// <summary>
		/// Amount the scroll wheel has changed since the previous frame
		/// </summary>
		public static float ScrollDelta => _aqua_internal_ScrollDelta();

		/// <summary>
		/// Current position of the mouse, in screen pixels relative to the top-left corner of the window
		/// </summary>
		public static Vector2 MousePosition
		{
			get
			{
				_aqua_internal_GetMousePosition(out Vector2 value);
				return value;
			}
			set => _aqua_internal_SetMousePosition(value);
		}

		/// <summary>
		/// Amount the mouse has moved, in pixels, since the previous frame
		/// </summary>
		public static Vector2 MouseDelta
		{
			get
			{
				_aqua_internal_GetMouseDelta(out Vector2 value);
				return value;
			}
		}

		/// <summary>
		/// Sets the mouse visibility & disabled state
		/// </summary>
		public static MouseState MouseState
		{
			get => (MouseState)_aqua_internal_GetMouseState();
			set => _aqua_internal_SetMouseState((int)value);
		}

		/// <summary>
		/// Checks if the mouse is showing. This can be changed using <see cref="MouseState"/>
		/// </summary>
		public static bool IsMouseShowing => _aqua_internal_IsMouseShowing();

		/// <returns>True if the button is not pressed down</returns>
		public static bool IsMouseUp(MouseButton button) => _aqua_internal_IsMouseUp((int)button);

		/// <returns>True if the button is currently pressed down</returns>
		public static bool IsMouseDown(MouseButton button) => _aqua_internal_IsMouseDown((int)button);

		/// <returns>True if the button is was pressed down this frame</returns>
		public static bool IsMousePressed(MouseButton button) => _aqua_internal_IsMousePressed((int)button);

		/// <returns>True if the button is was released this frame</returns>
		public static bool IsMouseReleased(MouseButton button) => _aqua_internal_IsMouseReleased((int)button);
		#endregion

		#region Gamepad
		/// <returns>True if a gamepad with index is connected & valid</returns>
		public static bool IsGamepadConnected(int gamepadIndex) => _aqua_internal_IsGamepadConnected(gamepadIndex);

		/// <returns>Value of axis on gamepad at <paramref name="gamepadIndex"/>, or 0 if not connected</returns>
		public static float GetAxis(int gamepadIndex, GamepadAxis axis) => _aqua_internal_GetAxis(gamepadIndex, (int)axis);

		/// <returns>True if button is not pressed on gamepad at <paramref name="gamepadIndex"/></returns>
		public static bool IsButtonUp(int gamepadIndex, GamepadButton button) => _aqua_internal_IsButtonUp(gamepadIndex, (int)button);

		/// <returns>True if button is pressed on gamepad at <paramref name="gamepadIndex"/></returns>
		public static bool IsButtonDown(int gamepadIndex, GamepadButton button) => _aqua_internal_IsButtonDown(gamepadIndex, (int)button);
		#endregion

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_InputEnabled();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_InputEnable(bool enable);

		// Keys
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_IsKeyUp(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_IsKeyDown(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_IsKeyPressed(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_IsKeyReleased(int key);

		// Mouse
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern float _aqua_internal_ScrollDelta();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _aqua_internal_GetMousePosition(out Vector2 output);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _aqua_internal_SetMousePosition(Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _aqua_internal_GetMouseDelta(out Vector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _aqua_internal_IsMouseUp(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _aqua_internal_IsMouseDown(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _aqua_internal_IsMousePressed(int key);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _aqua_internal_IsMouseReleased(int key);

		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern int _aqua_internal_GetMouseState();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _aqua_internal_IsMouseShowing();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void _aqua_internal_SetMouseState(int state);


		// Gamepad
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_IsGamepadConnected(int gamepadIndex);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _aqua_internal_GetAxis(int gamepadIndex, int axis);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_IsButtonUp(int gamepadIndex, int button);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _aqua_internal_IsButtonDown(int gamepadIndex, int button);
		#endregion
	}
}
