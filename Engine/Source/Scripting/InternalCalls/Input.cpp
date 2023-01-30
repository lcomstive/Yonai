#include <mono/jit/jit.h>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/InputKeys.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;

ADD_MANAGED_METHOD(Input, InputEnabled, bool)
{ return Input::IsEnabled(); }
ADD_MANAGED_METHOD(Input, InputEnable, void, (bool enable))
{ Input::Enable(enable); }

// Keys
ADD_MANAGED_METHOD(Input, IsKeyUp, bool, (int key))
{ return Input::IsKeyUp((Key)key); }
ADD_MANAGED_METHOD(Input, IsKeyDown, bool, (int key))
{ return Input::IsKeyDown((Key)key); }
ADD_MANAGED_METHOD(Input, IsKeyPressed, bool, (int key))
{ return Input::IsKeyPressed((Key)key); }
ADD_MANAGED_METHOD(Input, IsKeyReleased, bool, (int key))
{ return Input::IsKeyReleased((Key)key); }

// Mouse
ADD_MANAGED_METHOD(Input, ScrollDelta, float)
{ return Input::GetScrollDelta(); }
ADD_MANAGED_METHOD(Input, SetMousePosition, void, (glm::vec2 input))
{ Input::SetMousePosition(input); }
ADD_MANAGED_METHOD(Input, GetMouseDelta, void, (glm::vec2* output))
{ *output = Input::GetMouseDelta(); }
ADD_MANAGED_METHOD(Input, GetMousePosition, void, (glm::vec2* output))
{ *output = Input::GetMousePosition(); }

ADD_MANAGED_METHOD(Input, IsMouseUp, bool, (int button))
{ return Input::IsMouseUp(button); }
ADD_MANAGED_METHOD(Input, IsMouseDown, bool, (int button))
{ return Input::IsMouseDown(button); }
ADD_MANAGED_METHOD(Input, IsMousePressed, bool, (int button))
{ return Input::IsMousePressed(button); }
ADD_MANAGED_METHOD(Input, IsMouseReleased, bool, (int button))
{ return Input::IsMouseReleased(button); }

ADD_MANAGED_METHOD(Input, IsMouseShowing, bool)
{ return Input::IsMouseShowing(); }
ADD_MANAGED_METHOD(Input, GetMouseState, int)
{ return (int)Input::GetMouseState(); }
ADD_MANAGED_METHOD(Input, SetMouseState, void, (int state))
{ Input::SetMouseState((MouseState)state); }

// Gamepad
ADD_MANAGED_METHOD(Input, IsGamepadConnected, bool, (int index))
{ return Input::IsGamepadConnected(index); }
ADD_MANAGED_METHOD(Input, GetAxis, float, (int gamepadIndex, int axis))
{ return Input::GetAxis(gamepadIndex, (GamepadAxis)axis); }
ADD_MANAGED_METHOD(Input, IsButtonUp, bool, (int gamepadIndex, int button))
{ return Input::IsButtonUp(gamepadIndex, (GamepadButton)button); }
ADD_MANAGED_METHOD(Input, IsButtonDown, bool, (int gamepadIndex, int button))
{ return Input::IsButtonDown(gamepadIndex, (GamepadButton)button); }