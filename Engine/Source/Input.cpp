#include <map>
#include <spdlog/spdlog.h>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/Window.hpp>

using namespace std;
using namespace glm;
using namespace AquaEngine;

float Input::s_ScrollDelta = 0;
vec2 Input::s_MousePosition = { 0, 0 };
vec2 Input::s_LastMousePosition = { 0, 0 };
map<Key, Input::KeyState> Input::s_KeyStates;
map<int, Input::KeyState> Input::s_MouseStates;
map<int, Input::JoystickType> Input::s_ConnectedGamepads;

#pragma region Keys
bool Input::IsKeyUp(Key key)
{
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Up : true;
}

bool Input::IsKeyDown(Key key)
{
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Down : false;
}

bool Input::IsKeyPressed(Key key)
{
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Pressed : false;
}

bool Input::IsKeyReleased(Key key)
{
	auto it = s_KeyStates.find(key);
	return it != s_KeyStates.end() ? it->second == KeyState::Released : false;
}
#pragma endregion

#pragma region Mouse Buttons
bool Input::IsMouseUp(int key)
{
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Up : true;
}

bool Input::IsMouseDown(int key)
{
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Down : false;
}

bool Input::IsMousePressed(int key)
{
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Pressed : false;
}

bool Input::IsMouseReleased(int key)
{
	auto it = s_MouseStates.find(key);
	return it != s_MouseStates.end() ? it->second == KeyState::Released : false;
}
#pragma endregion

#pragma region Mouse
vec2 Input::GetMousePosition() { return s_MousePosition; }
vec2 Input::GetMouseDelta() { return (s_MousePosition - s_LastMousePosition); }

float Input::GetScrollDelta() { return s_ScrollDelta; }

void Input::ShowMouse(bool show)
{
#if defined(AQUA_PLATFORM_DESKTOP)
	glfwSetInputMode(Window::GetNativeHandle(), GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
#endif
}
#pragma endregion

#pragma region Gamepad
bool Input::IsGamepadConnected(int index)
{
#if defined(AQUA_PLATFORM_DESKTOP)
	return s_ConnectedGamepads[index] != JoystickType::Disconnected;
#endif
}

bool Input::IsButtonUp(int gamepadIndex, GamepadButton button)
{
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