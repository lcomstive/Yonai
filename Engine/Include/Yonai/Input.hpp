#pragma once
#include <map>
#include <glm/glm.hpp>
#include <Yonai/API.hpp>
#include <Yonai/InputKeys.hpp>

namespace Yonai
{
	class Input
	{
		enum class KeyState
		{
			/// <summary>
			/// Key is not pressed at all
			/// </summary>
			Up,

			/// <summary>
			/// Key is held down or pressed this frame
			/// </summary>
			Down,

			/// <summary>
			/// Key was pressed down this frame
			/// </summary>
			Pressed,

			/// <summary>
			/// Key was released during this frame
			/// </summary>
			Released
		};

		enum class JoystickType
		{
			Disconnected,
			Joystick,
			Gamepad
		};

		static bool s_Enabled;
		static float s_ScrollDelta;
		static MouseState s_MouseState;
		static glm::vec2 s_MousePosition;
		static glm::vec2 s_LastMousePosition;
		static std::map<Key, KeyState> s_KeyStates;
		static std::map<int, KeyState> s_MouseStates;
		static std::map<int, JoystickType> s_ConnectedGamepads;

		/// <summary>
		/// Iterates over all keys and updates their states.
		/// Intended to be called once per frame, from Window
		/// </summary>
		static void UpdateKeyStates();

		friend class Window;

	public:
		YonaiAPI static void Enable(bool enable);
		YonaiAPI static bool IsEnabled();

#pragma region Keys
		/// <returns>True if the key is not pressed down</returns>
		YonaiAPI static bool IsKeyUp(Key key);

		/// <returns>True if the key is currently pressed down</returns>
		YonaiAPI static bool IsKeyDown(Key key);

		/// <returns>True if the key is was pressed down this frame</returns>
		YonaiAPI static bool IsKeyPressed(Key key);

		/// <returns>True if the key is was released this frame</returns>
		YonaiAPI static bool IsKeyReleased(Key key);
#pragma endregion

#pragma region Mouse
		/// <returns>Mouse position relative to the top-left of the window</returns>
		YonaiAPI static glm::vec2 GetMousePosition();

		/// <summary>
		/// Sets the mouse position, in pixels relative to top-left of window
		/// </summary>
		YonaiAPI static void SetMousePosition(glm::vec2 value);

		/// <returns>Pixels moved since last frame</returns>
		YonaiAPI static glm::vec2 GetMouseDelta();

		/// <returns>Scroll offset since last frame</returns>
		YonaiAPI static float GetScrollDelta();

		YonaiAPI static bool IsMouseUp(int button);
		YonaiAPI static bool IsMouseDown(int button);
		YonaiAPI static bool IsMousePressed(int button);
		YonaiAPI static bool IsMouseReleased(int button);

		/// <summary>
		/// Sets the mouse visibility & disabled state
		/// </summary>
		YonaiAPI static void SetMouseState(MouseState state);

		/// <returns>Gets the mouse visibility & disabled state</returns>
		YonaiAPI static MouseState GetMouseState();

		/// <returns>Visible state of mouse</returns>
		YonaiAPI static bool IsMouseShowing();
#pragma endregion

#pragma region Gamepad
		/// <returns>State of gamepad connection at index</returns>
		YonaiAPI static bool IsGamepadConnected(int index);

		/// <returns>True if the button is not currently being pressed</returns>
		YonaiAPI static bool IsButtonUp(int gamepadIndex, GamepadButton button);

		/// <returns>True if the button is currently being pressed</returns>
		YonaiAPI static bool IsButtonDown(int gamepadIndex, GamepadButton button);

		/// <returns>Axis position, between -1.0 - 1.0</returns>
		YonaiAPI static float GetAxis(int gamepadIndex, GamepadAxis axis);
#pragma endregion
	};
}