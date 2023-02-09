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
unsigned int _managed_internal_ResourceDuplicate(unsigned int originalID, MonoString* newPath);
MonoString* _managed_internal_ResourceGetPath(unsigned int id);
unsigned int _managed_internal_ResourceGetID(MonoString* path);
void _managed_internal_ResourceUnload(unsigned int resourceID);
void* _managed_internal_ResourceGetInstance(unsigned int resourceID);
bool _managed_internal_ResourceExists(unsigned int resourceID);
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
MonoString* _managed_internal_VFSGetCurrentDirectory();
bool _managed_internal_VFSHasMount(MonoString* path);
void _managed_internal_VFSMount(MonoString* mountPoint);
void _managed_internal_VFSMountPath(MonoString* mountPoint, MonoString* mountPath);
void _managed_internal_VFSUnmount(MonoString* mountPoint);
void _managed_internal_VFSUnmountPath(MonoString* mountPoint, MonoString* mountPath);
MonoString* _managed_internal_VFSReadText(MonoString* path);
void _managed_internal_VFSWriteText(MonoString* path, MonoString* content);
MonoArray* _managed_internal_VFSRead(MonoString* path);
void _managed_internal_VFSWrite(MonoString* path, MonoArray* array);
void _managed_internal_VFSRemove(MonoString* path);
void _managed_internal_VFSMove(MonoString* from, MonoString* to);
void _managed_internal_VFSCopy(MonoString* from, MonoString* to);
MonoString* _managed_internal_VFSGetAbsolutePath(MonoString* path, bool suppressWarnings);
bool _managed_internal_VFSExists(MonoString* path);
float _managed_internal_TimeFPS();
float _managed_internal_TimeDeltaTime();
float _managed_internal_TimeTimeSinceLaunch();
void _managed_internal_ApplicationExit();
MonoString* _managed_internal_ApplicationGetArg(MonoString* name, MonoString* defaultValue);
bool _managed_internal_ApplicationHasArg(MonoString* name);
void _managed_internal_TextureLoad0(MonoString* path, unsigned int* outResourceID, void** outHandle);
void _managed_internal_TextureLoad1(MonoString* path, MonoString* filePath, bool hdr, unsigned int* outResourceID, void** outHandle);
void _managed_internal_TextureBind(void* instance, unsigned int index);


const vector<pair<const char*, const void*>> _InternalMethods = {
	pair<const char*, const void*>("AquaEngine.Log::_NativeLog", (const void*)_managed_internal_LogNativeLog),
	pair<const char*, const void*>("AquaEngine.Input::_InputEnabled", (const void*)_managed_internal_InputInputEnabled),
	pair<const char*, const void*>("AquaEngine.Input::_InputEnable", (const void*)_managed_internal_InputInputEnable),
	pair<const char*, const void*>("AquaEngine.Input::_IsKeyUp", (const void*)_managed_internal_InputIsKeyUp),
	pair<const char*, const void*>("AquaEngine.Input::_IsKeyDown", (const void*)_managed_internal_InputIsKeyDown),
	pair<const char*, const void*>("AquaEngine.Input::_IsKeyPressed", (const void*)_managed_internal_InputIsKeyPressed),
	pair<const char*, const void*>("AquaEngine.Input::_IsKeyReleased", (const void*)_managed_internal_InputIsKeyReleased),
	pair<const char*, const void*>("AquaEngine.Input::_ScrollDelta", (const void*)_managed_internal_InputScrollDelta),
	pair<const char*, const void*>("AquaEngine.Input::_SetMousePosition", (const void*)_managed_internal_InputSetMousePosition),
	pair<const char*, const void*>("AquaEngine.Input::_GetMouseDelta", (const void*)_managed_internal_InputGetMouseDelta),
	pair<const char*, const void*>("AquaEngine.Input::_GetMousePosition", (const void*)_managed_internal_InputGetMousePosition),
	pair<const char*, const void*>("AquaEngine.Input::_IsMouseUp", (const void*)_managed_internal_InputIsMouseUp),
	pair<const char*, const void*>("AquaEngine.Input::_IsMouseDown", (const void*)_managed_internal_InputIsMouseDown),
	pair<const char*, const void*>("AquaEngine.Input::_IsMousePressed", (const void*)_managed_internal_InputIsMousePressed),
	pair<const char*, const void*>("AquaEngine.Input::_IsMouseReleased", (const void*)_managed_internal_InputIsMouseReleased),
	pair<const char*, const void*>("AquaEngine.Input::_IsMouseShowing", (const void*)_managed_internal_InputIsMouseShowing),
	pair<const char*, const void*>("AquaEngine.Input::_GetMouseState", (const void*)_managed_internal_InputGetMouseState),
	pair<const char*, const void*>("AquaEngine.Input::_SetMouseState", (const void*)_managed_internal_InputSetMouseState),
	pair<const char*, const void*>("AquaEngine.Input::_IsGamepadConnected", (const void*)_managed_internal_InputIsGamepadConnected),
	pair<const char*, const void*>("AquaEngine.Input::_GetAxis", (const void*)_managed_internal_InputGetAxis),
	pair<const char*, const void*>("AquaEngine.Input::_IsButtonUp", (const void*)_managed_internal_InputIsButtonUp),
	pair<const char*, const void*>("AquaEngine.Input::_IsButtonDown", (const void*)_managed_internal_InputIsButtonDown),
	pair<const char*, const void*>("AquaEngine.Camera::_GetFOV", (const void*)_managed_internal_CameraGetFOV),
	pair<const char*, const void*>("AquaEngine.Camera::_SetFOV", (const void*)_managed_internal_CameraSetFOV),
	pair<const char*, const void*>("AquaEngine.Camera::_GetNear", (const void*)_managed_internal_CameraGetNear),
	pair<const char*, const void*>("AquaEngine.Camera::_SetNear", (const void*)_managed_internal_CameraSetNear),
	pair<const char*, const void*>("AquaEngine.Camera::_GetFar", (const void*)_managed_internal_CameraGetFar),
	pair<const char*, const void*>("AquaEngine.Camera::_SetFar", (const void*)_managed_internal_CameraSetFar),
	pair<const char*, const void*>("AquaEngine.Camera::_GetOrtho", (const void*)_managed_internal_CameraGetOrtho),
	pair<const char*, const void*>("AquaEngine.Camera::_SetOrtho", (const void*)_managed_internal_CameraSetOrtho),
	pair<const char*, const void*>("AquaEngine.Camera::_GetOrthoSize", (const void*)_managed_internal_CameraGetOrthoSize),
	pair<const char*, const void*>("AquaEngine.Camera::_SetOrthoSize", (const void*)_managed_internal_CameraSetOrthoSize),
	pair<const char*, const void*>("AquaEngine.Camera::_SetMainCamera", (const void*)_managed_internal_CameraSetMainCamera),
	pair<const char*, const void*>("AquaEngine.Camera::_GetMainCamera", (const void*)_managed_internal_CameraGetMainCamera),
	pair<const char*, const void*>("AquaEngine.Window::_SetResolution", (const void*)_managed_internal_WindowSetResolution),
	pair<const char*, const void*>("AquaEngine.Window::_GetResolution", (const void*)_managed_internal_WindowGetResolution),
	pair<const char*, const void*>("AquaEngine.Window::_SetFullscreenMode", (const void*)_managed_internal_WindowSetFullscreenMode),
	pair<const char*, const void*>("AquaEngine.Window::_GetFullscreenMode", (const void*)_managed_internal_WindowGetFullscreenMode),
	pair<const char*, const void*>("AquaEngine.Screen::_GetVideoModes", (const void*)_managed_internal_ScreenGetVideoModes),
	pair<const char*, const void*>("AquaEngine.Resource::_Duplicate", (const void*)_managed_internal_ResourceDuplicate),
	pair<const char*, const void*>("AquaEngine.Resource::_GetPath", (const void*)_managed_internal_ResourceGetPath),
	pair<const char*, const void*>("AquaEngine.Resource::_GetID", (const void*)_managed_internal_ResourceGetID),
	pair<const char*, const void*>("AquaEngine.Resource::_Unload", (const void*)_managed_internal_ResourceUnload),
	pair<const char*, const void*>("AquaEngine.Resource::_GetInstance", (const void*)_managed_internal_ResourceGetInstance),
	pair<const char*, const void*>("AquaEngine.Resource::_Exists", (const void*)_managed_internal_ResourceExists),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_GetSprite", (const void*)_managed_internal_SpriteRendererGetSprite),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_SetSprite", (const void*)_managed_internal_SpriteRendererSetSprite),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_GetShader", (const void*)_managed_internal_SpriteRendererGetShader),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_SetShader", (const void*)_managed_internal_SpriteRendererSetShader),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_GetColour", (const void*)_managed_internal_SpriteRendererGetColour),
	pair<const char*, const void*>("AquaEngine.SpriteRenderer::_SetColour", (const void*)_managed_internal_SpriteRendererSetColour),
	pair<const char*, const void*>("AquaEngine.Vector2::_Magnitude", (const void*)_managed_internal_Vector2Magnitude),
	pair<const char*, const void*>("AquaEngine.Vector2::_Distance", (const void*)_managed_internal_Vector2Distance),
	pair<const char*, const void*>("AquaEngine.Vector3::_Distance", (const void*)_managed_internal_Vector3Distance),
	pair<const char*, const void*>("AquaEngine.Vector3::_Dot", (const void*)_managed_internal_Vector3Dot),
	pair<const char*, const void*>("AquaEngine.Vector3::_Magnitude", (const void*)_managed_internal_Vector3Magnitude),
	pair<const char*, const void*>("AquaEngine.Vector4::_Magnitude", (const void*)_managed_internal_Vector4Magnitude),
	pair<const char*, const void*>("AquaEngine.Transform::_SetPosition", (const void*)_managed_internal_TransformSetPosition),
	pair<const char*, const void*>("AquaEngine.Transform::_GetPosition", (const void*)_managed_internal_TransformGetPosition),
	pair<const char*, const void*>("AquaEngine.Transform::_SetRotation", (const void*)_managed_internal_TransformSetRotation),
	pair<const char*, const void*>("AquaEngine.Transform::_GetRotation", (const void*)_managed_internal_TransformGetRotation),
	pair<const char*, const void*>("AquaEngine.Transform::_SetScale", (const void*)_managed_internal_TransformSetScale),
	pair<const char*, const void*>("AquaEngine.Transform::_GetScale", (const void*)_managed_internal_TransformGetScale),
	pair<const char*, const void*>("AquaEngine.Transform::_GetUp", (const void*)_managed_internal_TransformGetUp),
	pair<const char*, const void*>("AquaEngine.Transform::_GetForward", (const void*)_managed_internal_TransformGetForward),
	pair<const char*, const void*>("AquaEngine.Transform::_GetRight", (const void*)_managed_internal_TransformGetRight),
	pair<const char*, const void*>("AquaEngine.System::_Exists", (const void*)_managed_internal_SystemExists),
	pair<const char*, const void*>("AquaEngine.World::_Get", (const void*)_managed_internal_WorldGet),
	pair<const char*, const void*>("AquaEngine.World::_Exists", (const void*)_managed_internal_WorldExists),
	pair<const char*, const void*>("AquaEngine.World::_Destroy", (const void*)_managed_internal_WorldDestroy),
	pair<const char*, const void*>("AquaEngine.World::_Create", (const void*)_managed_internal_WorldCreate),
	pair<const char*, const void*>("AquaEngine.World::_CreateEntity", (const void*)_managed_internal_WorldCreateEntity),
	pair<const char*, const void*>("AquaEngine.World::_DestroyEntity", (const void*)_managed_internal_WorldDestroyEntity),
	pair<const char*, const void*>("AquaEngine.World::_HasEntity", (const void*)_managed_internal_WorldHasEntity),
	pair<const char*, const void*>("AquaEngine.World::_GetEntities", (const void*)_managed_internal_WorldGetEntities),
	pair<const char*, const void*>("AquaEngine.World::_GetComponents", (const void*)_managed_internal_WorldGetComponents),
	pair<const char*, const void*>("AquaEngine.World::_GetComponentsMultiple", (const void*)_managed_internal_WorldGetComponentsMultiple),
	pair<const char*, const void*>("AquaEngine.Entity::_HasComponent", (const void*)_managed_internal_EntityHasComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_AddComponent", (const void*)_managed_internal_EntityAddComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_RemoveComponent", (const void*)_managed_internal_EntityRemoveComponent),
	pair<const char*, const void*>("AquaEngine.Entity::_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	pair<const char*, const void*>("AquaEngine.World::_HasSystem", (const void*)_managed_internal_WorldHasSystem),
	pair<const char*, const void*>("AquaEngine.World::_GetSystem", (const void*)_managed_internal_WorldGetSystem),
	pair<const char*, const void*>("AquaEngine.World::_AddSystem", (const void*)_managed_internal_WorldAddSystem),
	pair<const char*, const void*>("AquaEngine.World::_RemoveSystem", (const void*)_managed_internal_WorldRemoveSystem),
	pair<const char*, const void*>("AquaEngine.VFS::_GetCurrentDirectory", (const void*)_managed_internal_VFSGetCurrentDirectory),
	pair<const char*, const void*>("AquaEngine.VFS::_HasMount", (const void*)_managed_internal_VFSHasMount),
	pair<const char*, const void*>("AquaEngine.VFS::_Mount", (const void*)_managed_internal_VFSMount),
	pair<const char*, const void*>("AquaEngine.VFS::_MountPath", (const void*)_managed_internal_VFSMountPath),
	pair<const char*, const void*>("AquaEngine.VFS::_Unmount", (const void*)_managed_internal_VFSUnmount),
	pair<const char*, const void*>("AquaEngine.VFS::_UnmountPath", (const void*)_managed_internal_VFSUnmountPath),
	pair<const char*, const void*>("AquaEngine.VFS::_ReadText", (const void*)_managed_internal_VFSReadText),
	pair<const char*, const void*>("AquaEngine.VFS::_WriteText", (const void*)_managed_internal_VFSWriteText),
	pair<const char*, const void*>("AquaEngine.VFS::_Read", (const void*)_managed_internal_VFSRead),
	pair<const char*, const void*>("AquaEngine.VFS::_Write", (const void*)_managed_internal_VFSWrite),
	pair<const char*, const void*>("AquaEngine.VFS::_Remove", (const void*)_managed_internal_VFSRemove),
	pair<const char*, const void*>("AquaEngine.VFS::_Move", (const void*)_managed_internal_VFSMove),
	pair<const char*, const void*>("AquaEngine.VFS::_Copy", (const void*)_managed_internal_VFSCopy),
	pair<const char*, const void*>("AquaEngine.VFS::_GetAbsolutePath", (const void*)_managed_internal_VFSGetAbsolutePath),
	pair<const char*, const void*>("AquaEngine.VFS::_Exists", (const void*)_managed_internal_VFSExists),
	pair<const char*, const void*>("AquaEngine.Time::_FPS", (const void*)_managed_internal_TimeFPS),
	pair<const char*, const void*>("AquaEngine.Time::_DeltaTime", (const void*)_managed_internal_TimeDeltaTime),
	pair<const char*, const void*>("AquaEngine.Time::_TimeSinceLaunch", (const void*)_managed_internal_TimeTimeSinceLaunch),
	pair<const char*, const void*>("AquaEngine.Application::_Exit", (const void*)_managed_internal_ApplicationExit),
	pair<const char*, const void*>("AquaEngine.Application::_GetArg", (const void*)_managed_internal_ApplicationGetArg),
	pair<const char*, const void*>("AquaEngine.Application::_HasArg", (const void*)_managed_internal_ApplicationHasArg),
	pair<const char*, const void*>("AquaEngine.Graphics.Texture::_Load0", (const void*)_managed_internal_TextureLoad0),
	pair<const char*, const void*>("AquaEngine.Graphics.Texture::_Load1", (const void*)_managed_internal_TextureLoad1),
	pair<const char*, const void*>("AquaEngine.Graphics.Texture::_Bind", (const void*)_managed_internal_TextureBind)
};