#pragma once
#include <map>
#include <glm/glm.hpp>
#include <AquaEngine/API.hpp>
#include <AquaEngine/InputKeys.hpp>

namespace AquaEngine
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
		AquaAPI static void Enable(bool enable);
		AquaAPI static bool IsEnabled();

#pragma region Keys
		/// <returns>True if the key is not pressed down</returns>
		AquaAPI static bool IsKeyUp(Key key);

		/// <returns>True if the key is currently pressed down</returns>
		AquaAPI static bool IsKeyDown(Key key);

		/// <returns>True if the key is was pressed down this frame</returns>
		AquaAPI static bool IsKeyPressed(Key key);

		/// <returns>True if the key is was released this frame</returns>
		AquaAPI static bool IsKeyReleased(Key key);
#pragma endregion

#pragma region Mouse
		/// <returns>Mouse position relative to the top-left of the window</returns>
		AquaAPI static glm::vec2 GetMousePosition();

		/// <returns>Pixels moved since last frame</returns>
		AquaAPI static glm::vec2 GetMouseDelta();

		/// <returns>Scroll offset since last frame</returns>
		AquaAPI static float GetScrollDelta();

		AquaAPI static bool IsMouseUp(int button);
		AquaAPI static bool IsMouseDown(int button);
		AquaAPI static bool IsMousePressed(int button);
		AquaAPI static bool IsMouseReleased(int button);

		/// <summary>
		/// Can hide or show the mouse. When hidden mouse is locked and cannot exit window.
		/// </summary>
		AquaAPI static void ShowMouse(bool show = true);
#pragma endregion

#pragma region Gamepad
		/// <returns>State of gamepad connection at index</returns>
		AquaAPI static bool IsGamepadConnected(int index);

		/// <returns>True if the button is not currently being pressed</returns>
		AquaAPI static bool IsButtonUp(int gamepadIndex, GamepadButton button);

		/// <returns>True if the button is currently being pressed</returns>
		AquaAPI static bool IsButtonDown(int gamepadIndex, GamepadButton button);

		/// <returns>Axis position, between -1.0 - 1.0</returns>
		AquaAPI static float GetAxis(int gamepadIndex, GamepadAxis axis);
#pragma endregion
	};
}