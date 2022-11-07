#include <mono/jit/jit.h>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/InputKeys.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>

using namespace AquaEngine;

bool InputEnabled() { return Input::IsEnabled(); }
void InputEnable(bool enable) { Input::Enable(enable); }

// Keys
bool IsKeyUp(int key) { return Input::IsKeyUp((Key)key); }
bool IsKeyDown(int key) { return Input::IsKeyDown((Key)key); }
bool IsKeyPressed(int key) { return Input::IsKeyPressed((Key)key); }
bool IsKeyReleased(int key) { return Input::IsKeyReleased((Key)key); }

// Mouse
float ScrollDelta() { return Input::GetScrollDelta(); }
void SetMousePosition(glm::vec2 input) { Input::SetMousePosition(input); }
void GetMouseDelta(glm::vec2* output) { *output = Input::GetMouseDelta(); }
void GetMousePosition(glm::vec2* output) { *output = Input::GetMousePosition(); }

bool IsMouseUp(int button) { return Input::IsMouseUp(button); }
bool IsMouseDown(int button) { return Input::IsMouseDown(button); }
bool IsMousePressed(int button) { return Input::IsMousePressed(button); }
bool IsMouseReleased(int button) { return Input::IsMouseReleased(button); }

bool IsMouseShowing() { return Input::IsMouseShowing(); }
int GetMouseState() { return (int)Input::GetMouseState(); }
void SetMouseState(int state) { Input::SetMouseState((MouseState)state); }

// Gamepad
bool IsGamepadConnected(int index) { return Input::IsGamepadConnected(index); }
float GetAxis(int gamepadIndex, int axis) { return Input::GetAxis(gamepadIndex, (GamepadAxis)axis); }
bool IsButtonUp(int gamepadIndex, int button) { return Input::IsButtonUp(gamepadIndex, (GamepadButton)button); }
bool IsButtonDown(int gamepadIndex, int button) { return Input::IsButtonDown(gamepadIndex, (GamepadButton)button); }

#define ADD_INPUT_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.Input::_aqua_internal_"#name, (const void*)name);

void AquaEngine::Scripting::Assembly::AddInputInternalCalls()
{
	ADD_INPUT_INTERNAL_CALL(InputEnable)
	ADD_INPUT_INTERNAL_CALL(InputEnabled)

	// Keys
	ADD_INPUT_INTERNAL_CALL(IsKeyUp)
	ADD_INPUT_INTERNAL_CALL(IsKeyDown)
	ADD_INPUT_INTERNAL_CALL(IsKeyPressed)
	ADD_INPUT_INTERNAL_CALL(IsKeyReleased)

	// Mouse
	ADD_INPUT_INTERNAL_CALL(ScrollDelta)
	ADD_INPUT_INTERNAL_CALL(GetMouseDelta)
	ADD_INPUT_INTERNAL_CALL(GetMousePosition)
	ADD_INPUT_INTERNAL_CALL(SetMousePosition)

	ADD_INPUT_INTERNAL_CALL(IsMouseUp)
	ADD_INPUT_INTERNAL_CALL(IsMouseDown)
	ADD_INPUT_INTERNAL_CALL(IsMousePressed)
	ADD_INPUT_INTERNAL_CALL(IsMouseReleased)

	ADD_INPUT_INTERNAL_CALL(SetMouseState)
	ADD_INPUT_INTERNAL_CALL(GetMouseState)
	ADD_INPUT_INTERNAL_CALL(IsMouseShowing)

	// Gamepad
	ADD_INPUT_INTERNAL_CALL(GetAxis)
	ADD_INPUT_INTERNAL_CALL(IsButtonUp)
	ADD_INPUT_INTERNAL_CALL(IsButtonDown)
	ADD_INPUT_INTERNAL_CALL(IsGamepadConnected)
}