#include <map>
#include <spdlog/spdlog.h>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/Window.hpp>

using namespace std;
using namespace glm;
using namespace AquaEngine;

bool Input::s_Enabled = true;
float Input::s_ScrollDelta = 0;
vec2 Input::s_MousePosition = { 0, 0 };
vec2 Input::s_LastMousePosition = { 0, 0 };
map<Key, Input::KeyState> Input::s_KeyStates;
map<int, Input::KeyState> Input::s_MouseStates;
MouseState Input::s_MouseState = MouseState::Normal;
map<int, Input::JoystickType> Input::s_ConnectedGamepads;

bool Input::IsEnabled() { return s_Enabled; }
void Input::Enable(bool enable) { s_Enabled = enable; }

#pragma region Keys
bool Input::IsKeyUp(Key key)
{
	if (!s_Enabled)
		return true;
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Up : true;
}

bool Input::IsKeyDown(Key key)
{
	if (!s_Enabled)
		return false;
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Down : false;
}

bool Input::IsKeyPressed(Key key)
{
	if (!s_Enabled)
		return false;
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Pressed : false;
}

bool Input::IsKeyReleased(Key key)
{
	if (!s_Enabled)
		return false;
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Released : false;
}
#pragma endregion

#pragma region Mouse Buttons
bool Input::IsMouseUp(int key)
{
	if (!s_Enabled)
		return true;
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Up : true;
}

bool Input::IsMouseDown(int key)
{
	if (!s_Enabled)
		return false;
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Down : false;
}

bool Input::IsMousePressed(int key)
{
	if (!s_Enabled)
		return false;
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Pressed : false;
}

bool Input::IsMouseReleased(int key)
{
	if (!s_Enabled)
		return false;
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Released : false;
}
#pragma endregion

#pragma region Mouse
vec2 Input::GetMousePosition() { return s_Enabled ? s_MousePosition : vec2(); }
vec2 Input::GetMouseDelta() { return s_Enabled ? (s_MousePosition - s_LastMousePosition) : vec2(); }
void Input::SetMousePosition(glm::vec2 value) { if(s_Enabled) glfwSetCursorPos(Window::GetNativeHandle(), value.x, value.y); }

float Input::GetScrollDelta() { return s_Enabled ? s_ScrollDelta : 0; }

void Input::SetMouseState(MouseState state)
{
	if (!s_Enabled || !Window::GetNativeHandle())
		return;

#if defined(AQUA_PLATFORM_DESKTOP)
	int glfwState = GLFW_CURSOR_NORMAL;
	if (state == MouseState::Hidden) glfwState = GLFW_CURSOR_HIDDEN;
	else if (state == MouseState::Disabled) glfwState = GLFW_CURSOR_DISABLED;

	glfwSetInputMode(Window::GetNativeHandle(), GLFW_CURSOR, glfwState);

	s_MouseState = state;
#endif
}

MouseState Input::GetMouseState() { return s_MouseState; }
bool Input::IsMouseShowing() { return s_MouseState == MouseState::Normal; }
#pragma endregion

#pragma region Gamepad
bool Input::IsGamepadConnected(int index)
{
	if (!s_Enabled)
		return false;

#if defined(AQUA_PLATFORM_DESKTOP)
	return s_ConnectedGamepads[index] != JoystickType::Disconnected;
#endif
}

bool Input::IsButtonUp(int gamepadIndex, GamepadButton button)
{
	if (!s_Enabled)
		return true;

#if defined(AQUA_PLATFORM_DESKTOP)
	GLFWgamepadstate state;
	if (!glfwGetGamepadState(gamepadIndex, &state))
		return true; // Gamepad not found. Emulate key being up
	return state.buttons[(int)button] == GLFW_RELEASE;
#endif
	return true;
}

bool Input::IsButtonDown(int gamepadIndex, GamepadButton button)
{
	if (!s_Enabled)
		return false;

#if defined(AQUA_PLATFORM_DESKTOP)
	GLFWgamepadstate state;
	if (!glfwGetGamepadState(gamepadIndex, &state))
		return false; // Gamepad not found. Emulate key being up
	return state.buttons[(int)button] == GLFW_PRESS;
#endif
	return true;
}

float Input::GetAxis(int gamepadIndex, GamepadAxis axis)
{
	if (!s_Enabled)
		return 0;

#if defined(AQUA_PLATFORM_DESKTOP)
	GLFWgamepadstate state;
	if (!glfwGetGamepadState(gamepadIndex, &state))
		return 0; // Gamepad not found. Emulate axis being neutral
	return state.axes[(int)axis];
#endif
}

#pragma endregion

void Input::UpdateKeyStates()
{
	// Loop through all s_KeyStates and check their current state
	for (auto& pair : s_KeyStates)
	{
		switch (pair.second)
		{
			// Keys currently pressed are now classed as down/held
		case KeyState::Pressed:
			s_KeyStates[pair.first] = KeyState::Down;
			break;

			// Keys that have been released are now classed as up
		case KeyState::Released:
			s_KeyStates[pair.first] = KeyState::Up;
			break;
		default: break;
		}
	}

	// Loop through all s_MouseStates and check their current state
	for (auto& pair : s_MouseStates)
	{
		switch (pair.second)
		{
			// Keys currently pressed are now classed as down/held
		case KeyState::Pressed:
			s_MouseStates[pair.first] = KeyState::Down;
			break;

			// Keys that have been released are now classed as up
		case KeyState::Released:
			s_MouseStates[pair.first] = KeyState::Up;
			break;
		default: break;
		}
	}

	// Set current mouse position as "last" mouse position
	s_LastMousePosition = s_MousePosition;

	// Reset scroll delta every frame
	s_ScrollDelta = 0;
}