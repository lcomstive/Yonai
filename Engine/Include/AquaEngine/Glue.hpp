#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

using namespace std; 

void _managed_internal_LogNativeLog(MonoString* rawMsg, int level);
bool _managed_internal_InputInputEnabled();
void _managed_internal_InputInputEnable(bool enable);
bool _managed_internal_InputIsKeyUp(int key);
bool _managed_internal_InputIsKeyDown(int key);
bool _managed_internal_InputIsKeyPressed(int key);
bool _managed_internal_InputIsKeyReleased(int key);
float _managed_internal_InputScrollDelta();
void _managed_internal_InputSetMousePosition(glm::vec2 input);
void _managed_internal_InputGetMouseDelta(glm::vec2* output);
void _managed_internal_InputGetMousePosition(glm::vec2* output);
bool _managed_internal_InputIsMouseUp(int button);
bool _managed_internal_InputIsMouseDown(int button);
bool _managed_internal_InputIsMousePressed(int button);
bool _managed_internal_InputIsMouseReleased(int button);
bool _managed_internal_InputIsMouseShowing();
int _managed_internal_InputGetMouseState();
void _managed_internal_InputSetMouseState(int state);
bool _managed_internal_InputIsGamepadConnected(int index);
float _managed_internal_InputGetAxis(int gamepadIndex, int axis);
bool _managed_internal_InputIsButtonUp(int gamepadIndex, int button);
bool _managed_internal_InputIsButtonDown(int gamepadIndex, int button);
float _managed_internal_CameraGetFOV(unsigned int worldID, unsigned int entityID);
void _managed_internal_CameraSetFOV(unsigned int worldID, unsigned int entityID, float value);
float _managed_internal_CameraGetNear(unsigned int worldID, unsigned int entityID);
void _managed_internal_CameraSetNear(unsigned int worldID, unsigned int entityID, float value);
float _managed_internal_CameraGetFar(unsigned int worldID, unsigned int entityID);
void _managed_internal_CameraSetFar(unsigned int worldID, unsigned int entityID, float value);
float _managed_internal_CameraGetOrtho(unsigned int worldID, unsigned int entityID);
void _managed_internal_CameraSetOrtho(unsigned int worldID, unsigned int entityID, bool value);
float _managed_internal_CameraGetOrthoSize(unsigned int worldID, unsigned int entityID);
void _managed_internal_CameraSetOrthoSize(unsigned int worldID, unsigned int entityID, float value);
void _managed_internal_CameraSetMainCamera(unsigned int worldID, unsigned int entityID);
void _managed_internal_CameraGetMainCamera(unsigned int* worldID, unsigned int* entityID);
void _managed_internal_WindowSetResolution(glm::vec2* value);
void _managed_internal_WindowGetResolution(glm::vec2* outValue);
void _managed_internal_WindowSetFullscreenMode(int state);
int _managed_internal_WindowGetFullscreenMode();
int _managed_internal_ScreenGetVideoModes(MonoArray** outBits, MonoArray** outWidths, MonoArray** outHeights, MonoArray** outRefreshRates);
unsigned int _managed_internal_SpriteRendererGetSprite(unsigned int worldID, unsigned int entityID);
void _managed_internal_SpriteRendererSetSprite(unsigned int worldID, unsigned int entityID, unsigned int sprite);
unsigned int _managed_internal_SpriteRendererGetShader(unsigned int worldID, unsigned int entityID);
void _managed_internal_SpriteRendererSetShader(unsigned int worldID, unsigned int entityID, unsigned int shader);
void _managed_internal_SpriteRendererGetColour(unsigned int worldID, unsigned int entityID, glm::vec4* colour);
void _managed_internal_SpriteRendererSetColour(unsigned int worldID, unsigned int entityID, glm::vec4* colour);
float _managed_internal_Vector2Magnitude(glm::vec2* input);
float _managed_internal_Vector2Distance(glm::vec2* a, glm::vec2* b);
float _managed_internal_Vector3Distance(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Dot(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Magnitude(glm::vec3* input);
float _managed_internal_Vector4Magnitude(glm::vec4* input);
void _managed_internal_TransformSetPosition(unsigned int worldID, unsigned int entityID, glm::vec3* value);
void _managed_internal_TransformGetPosition(unsigned int worldID, unsigned int entityID, glm::vec3* outValue);
void _managed_internal_TransformSetRotation(unsigned int worldID, unsigned int entityID, glm::quat* value);
void _managed_internal_TransformGetRotation(unsigned int worldID, unsigned int entityID, glm::quat* outValue);
void _managed_internal_TransformSetScale(unsigned int worldID, unsigned int entityID, glm::vec3* value);
void _managed_internal_TransformGetScale(unsigned int worldID, unsigned int entityID, glm::vec3* outValue);
void _managed_internal_TransformGetUp(unsigned int worldID, unsigned int entityID, glm::vec3* outValue);
void _managed_internal_TransformGetForward(unsigned int worldID, unsigned int entityID, glm::vec3* outValue);
void _managed_internal_TransformGetRight(unsigned int worldID, unsigned int entityID, glm::vec3* outValue);
bool _managed_internal_SystemExists(unsigned int worldID, MonoReflectionType* componentType);
size_t _managed_internal_WorldGetComponentType(MonoReflectionType* componentType);
bool _managed_internal_WorldGet(unsigned int worldID, MonoString** outName);
bool _managed_internal_WorldExists(unsigned int worldID);
bool _managed_internal_WorldDestroy(unsigned int worldID);
bool _managed_internal_WorldCreate(MonoString* name);
unsigned int _managed_internal_WorldCreateEntity(unsigned int worldID);
void _managed_internal_WorldDestroyEntity(unsigned int worldID, unsigned int entityID);
bool _managed_internal_WorldHasEntity(unsigned int worldID, unsigned int entityID);
MonoArray* _managed_internal_WorldGetEntities(unsigned int worldID);
MonoArray* _managed_internal_WorldGetComponents(unsigned int worldID, MonoReflectionType* componentType);
MonoArray* _managed_internal_WorldGetComponentsMultiple(unsigned int worldID, MonoArray* inputTypes);
bool _managed_internal_EntityHasComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType);
MonoObject* _managed_internal_EntityGetComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType);
MonoObject* _managed_internal_EntityAddComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType);
bool _managed_internal_EntityRemoveComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType);
MonoObject* _managed_internal_EntityGetComponent(unsigned int worldID, unsigned int entityID, MonoReflectionType* componentType);
bool _managed_internal_WorldHasSystem(unsigned int worldID, MonoReflectionType* systemType);
MonoObject* _managed_internal_WorldGetSystem(unsigned int worldID, MonoReflectionType* systemType);
MonoObject* _managed_internal_WorldAddSystem(unsigned int worldID, MonoReflectionType* systemType);
bool _managed_internal_WorldRemoveSystem(unsigned int worldID, MonoReflectionType* systemType);
float _managed_internal_TimeFPS();
float _managed_internal_TimeDeltaTime();
float _managed_internal_TimeTimeSinceLaunch();
void _managed_internal_ApplicationExit();
MonoString* _managed_internal_ApplicationGetArg(MonoString* name, MonoString* defaultValue);
bool _managed_internal_ApplicationHasArg(MonoString* name);


const vector<pair<const char*, const void*>> _InternalMethods = {
	pair<const char*, const void*>("AquaEngine.Log::_aqua_internal_NativeLog", (const void*)_managed_internal_LogNativeLog),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_InputEnabled", (const void*)_managed_internal_InputInputEnabled),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_InputEnable", (const void*)_managed_internal_InputInputEnable),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsKeyUp", (const void*)_managed_internal_InputIsKeyUp),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsKeyDown", (const void*)_managed_internal_InputIsKeyDown),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsKeyPressed", (const void*)_managed_internal_InputIsKeyPressed),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsKeyReleased", (const void*)_managed_internal_InputIsKeyReleased),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_ScrollDelta", (const void*)_managed_internal_InputScrollDelta),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_SetMousePosition", (const void*)_managed_internal_InputSetMousePosition),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_GetMouseDelta", (const void*)_managed_internal_InputGetMouseDelta),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_GetMousePosition", (const void*)_managed_internal_InputGetMousePosition),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsMouseUp", (const void*)_managed_internal_InputIsMouseUp),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsMouseDown", (const void*)_managed_internal_InputIsMouseDown),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsMousePressed", (const void*)_managed_internal_InputIsMousePressed),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsMouseReleased", (const void*)_managed_internal_InputIsMouseReleased),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsMouseShowing", (const void*)_managed_internal_InputIsMouseShowing),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_GetMouseState", (const void*)_managed_internal_InputGetMouseState),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_SetMouseState", (const void*)_managed_internal_InputSetMouseState),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsGamepadConnected", (const void*)_managed_internal_InputIsGamepadConnected),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_GetAxis", (const void*)_managed_internal_InputGetAxis),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsButtonUp", (const void*)_managed_internal_InputIsButtonUp),
	pair<const char*, const void*>("AquaEngine.Input::_aqua_internal_IsButtonDown", (const void*)_managed_internal_InputIsButtonDown),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_GetFOV", (const void*)_managed_internal_CameraGetFOV),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_SetFOV", (const void*)_managed_internal_CameraSetFOV),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_GetNear", (const void*)_managed_internal_CameraGetNear),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_SetNear", (const void*)_managed_internal_CameraSetNear),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_GetFar", (const void*)_managed_internal_CameraGetFar),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_SetFar", (const void*)_managed_internal_CameraSetFar),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_GetOrtho", (const void*)_managed_internal_CameraGetOrtho),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_SetOrtho", (const void*)_managed_internal_CameraSetOrtho),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_GetOrthoSize", (const void*)_managed_internal_CameraGetOrthoSize),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_SetOrthoSize", (const void*)_managed_internal_CameraSetOrthoSize),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_SetMainCamera", (const void*)_managed_internal_CameraSetMainCamera),
	pair<const char*, const void*>("AquaEngine.Camera::_aqua_internal_GetMainCamera", (const void*)_managed_internal_CameraGetMainCamera),
	pair<const char*, const void*>("AquaEngine.Window::_aqua_internal_SetResolution", (const void*)_managed_internal_WindowSetResolution),
	pair<const char*, const void*>("AquaEngine.Window::_aqua_internal_GetResolution", (const void*)_managed_internal_WindowGetResolution),
	pair<const char*, const void*>("AquaEngine.Window::_aqua_internal_SetFullscreenMode", (const void*)_managed_internal_WindowSetFullscreenMode),
	pair<const char*, const void*>("AquaEngine.Window::_aqua_internal_GetFullscreenMode", (const void*)_managed_internal_WindowGetFullscreenMode),
	pair<const char*, const void*>("AquaEngine.Screen::_aqua_internal_GetVideoModes", (const void*)_managed_internal_ScreenGetVideoModes),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_aqua_internal_GetSprite", (const void*)_managed_internal_SpriteRendererGetSprite),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_aqua_internal_SetSprite", (const void*)_managed_internal_SpriteRendererSetSprite),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_aqua_internal_GetShader", (const void*)_managed_internal_SpriteRendererGetShader),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_aqua_internal_SetShader", (const void*)_managed_internal_SpriteRendererSetShader),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_aqua_internal_GetColour", (const void*)_managed_internal_SpriteRendererGetColour),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_aqua_internal_SetColour", (const void*)_managed_internal_SpriteRendererSetColour),
	pair<const char*, const void*>("AquaEngine.Vector2::_aqua_internal_Magnitude", (const void*)_managed_internal_Vector2Magnitude),
	pair<const char*, const void*>("AquaEngine.Vector2::_aqua_internal_Distance", (const void*)_managed_internal_Vector2Distance),
	pair<const char*, const void*>("AquaEngine.Vector3::_aqua_internal_Distance", (const void*)_managed_internal_Vector3Distance),
	pair<const char*, const void*>("AquaEngine.Vector3::_aqua_internal_Dot", (const void*)_managed_internal_Vector3Dot),
	pair<const char*, const void*>("AquaEngine.Vector3::_aqua_internal_Magnitude", (const void*)_managed_internal_Vector3Magnitude),
	pair<const char*, const void*>("AquaEngine.Vector4::_aqua_internal_Magnitude", (const void*)_managed_internal_Vector4Magnitude),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_SetPosition", (const void*)_managed_internal_TransformSetPosition),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_GetPosition", (const void*)_managed_internal_TransformGetPosition),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_SetRotation", (const void*)_managed_internal_TransformSetRotation),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_GetRotation", (const void*)_managed_internal_TransformGetRotation),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_SetScale", (const void*)_managed_internal_TransformSetScale),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_GetScale", (const void*)_managed_internal_TransformGetScale),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_GetUp", (const void*)_managed_internal_TransformGetUp),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_GetForward", (const void*)_managed_internal_TransformGetForward),
	pair<const char*, const void*>("AquaEngine.Transform::_aqua_internal_GetRight", (const void*)_managed_internal_TransformGetRight),
	pair<const char*, const void*>("AquaEngine.System::_aqua_internal_Exists", (const void*)_managed_internal_SystemExists),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_GetComponentType", (const void*)_managed_internal_WorldGetComponentType),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_Get", (const void*)_managed_internal_WorldGet),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_Exists", (const void*)_managed_internal_WorldExists),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_Destroy", (const void*)_managed_internal_WorldDestroy),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_Create", (const void*)_managed_internal_WorldCreate),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_CreateEntity", (const void*)_managed_internal_WorldCreateEntity),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_DestroyEntity", (const void*)_managed_internal_WorldDestroyEntity),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_HasEntity", (const void*)_managed_internal_WorldHasEntity),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_GetEntities", (const void*)_managed_internal_WorldGetEntities),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_GetComponents", (const void*)_managed_internal_WorldGetComponents),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_GetComponentsMultiple", (const void*)_managed_internal_WorldGetComponentsMultiple),
	pair<const char*, const void*>("AquaEngine.Entity::_aqua_internal_HasComponent", (const void*)_managed_internal_EntityHasComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_aqua_internal_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_aqua_internal_AddComponent", (const void*)_managed_internal_EntityAddComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_aqua_internal_RemoveComponent", (const void*)_managed_internal_EntityRemoveComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_aqua_internal_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_HasSystem", (const void*)_managed_internal_WorldHasSystem),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_GetSystem", (const void*)_managed_internal_WorldGetSystem),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_AddSystem", (const void*)_managed_internal_WorldAddSystem),
	pair<const char*, const void*>("AquaEngine.World::_aqua_internal_RemoveSystem", (const void*)_managed_internal_WorldRemoveSystem),
	pair<const char*, const void*>("AquaEngine.Time::_aqua_internal_FPS", (const void*)_managed_internal_TimeFPS),
	pair<const char*, const void*>("AquaEngine.Time::_aqua_internal_DeltaTime", (const void*)_managed_internal_TimeDeltaTime),
	pair<const char*, const void*>("AquaEngine.Time::_aqua_internal_TimeSinceLaunch", (const void*)_managed_internal_TimeTimeSinceLaunch),
	pair<const char*, const void*>("AquaEngine.Application::_aqua_internal_Exit", (const void*)_managed_internal_ApplicationExit),
	pair<const char*, const void*>("AquaEngine.Application::_aqua_internal_GetArg", (const void*)_managed_internal_ApplicationGetArg),
	pair<const char*, const void*>("AquaEngine.Application::_aqua_internal_HasArg", (const void*)_managed_internal_ApplicationHasArg)
};