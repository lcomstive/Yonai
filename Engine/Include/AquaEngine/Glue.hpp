#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

using namespace std; 

typedef pair<const char*, const void*> MethodType; 

void _managed_internal_ApplicationExit();
MonoString* _managed_internal_ApplicationGetArg(MonoString* name, MonoString* defaultValue);
bool _managed_internal_ApplicationHasArg(MonoString* name);
float _managed_internal_CameraGetFar(void* handle);
void _managed_internal_CameraSetFar(void* handle, float value);
float _managed_internal_CameraGetNear(void* handle);
void _managed_internal_CameraSetNear(void* handle, float value);
float _managed_internal_CameraGetFieldOfView(void* handle);
void _managed_internal_CameraSetFieldOfView(void* handle, float value);
bool _managed_internal_CameraGetOrthographic(void* handle);
void _managed_internal_CameraSetOrthographic(void* handle, bool value);
float _managed_internal_CameraGetOrthographicSize(void* handle);
void _managed_internal_CameraSetOrthographicSize(void* handle, float value);
void* _managed_internal_NativeComponentGetHandle(unsigned int worldID, unsigned int entityID, MonoReflectionType* type);
unsigned int _managed_internal_MeshRendererGetMesh(void* handle);
void _managed_internal_MeshRendererSetMesh(void* handle, unsigned int value);
unsigned int _managed_internal_MeshRendererGetMaterial(void* handle);
void _managed_internal_MeshRendererSetMaterial(void* handle, unsigned int value);
void _managed_internal_MaterialLoad(MonoString* path, unsigned int* resourceID, void** outHandle);
void _managed_internal_MaterialGetAlbedo(void* handle, glm::vec4* value);
void _managed_internal_MaterialSetAlbedo(void* handle, glm::vec4* value);
void _managed_internal_MaterialGetTextureCoordinateScale(void* handle, glm::vec2* value);
void _managed_internal_MaterialSetTextureCoordinateScale(void* handle, glm::vec2* value);
void _managed_internal_MaterialGetTextureCoordinateOffset(void* handle, glm::vec2* value);
void _managed_internal_MaterialSetTextureCoordinateOffset(void* handle, glm::vec2* value);
unsigned int _managed_internal_MaterialGetShader(void* handle);
void _managed_internal_MaterialSetShader(void* handle, unsigned int value);
unsigned int _managed_internal_MaterialGetAlbedoMap(void* handle);
void _managed_internal_MaterialSetAlbedoMap(void* handle, unsigned int value);
bool _managed_internal_MaterialGetAlphaClipping(void* handle);
void _managed_internal_MaterialSetAlphaClipping(void* handle, bool value);
float _managed_internal_MaterialGetAlphaClipThreshold(void* handle);
void _managed_internal_MaterialSetAlphaClipThreshold(void* handle, float value);
float _managed_internal_MaterialGetRoughness(void* handle);
void _managed_internal_MaterialSetRoughness(void* handle, float value);
unsigned int _managed_internal_MaterialGetRoughnessMap(void* handle);
void _managed_internal_MaterialSetRoughnessMap(void* handle, unsigned int value);
float _managed_internal_MaterialGetMetalness(void* handle);
void _managed_internal_MaterialSetMetalness(void* handle, float value);
unsigned int _managed_internal_MaterialGetMetalnessMap(void* handle);
void _managed_internal_MaterialSetMetalnessMap(void* handle, unsigned int value);
unsigned int _managed_internal_MaterialGetNormalMap(void* handle);
void _managed_internal_MaterialSetNormalMap(void* handle, unsigned int value);
unsigned int _managed_internal_MaterialGetAmbientOcclusionMap(void* handle);
void _managed_internal_MaterialSetAmbientOcclusionMap(void* handle, unsigned int value);
bool _managed_internal_MaterialGetTransparent(void* handle);
void _managed_internal_MaterialSetTransparent(void* handle, bool value);
void _managed_internal_MeshLoad0(MonoString* path, unsigned int* outID, void** outHandle);
void _managed_internal_MeshLoad1(MonoString* path, MonoArray* inVertices, MonoArray* inIndices,  unsigned char drawMode, unsigned int* outID, void** outHandle);
void _managed_internal_MeshGetVertices(void* handle, MonoArray** outPositions, MonoArray** outNormals, MonoArray** outTexCoords);
void _managed_internal_MeshSetVertices(void* handle, MonoArray* inPositions, MonoArray* inNormals, MonoArray* inTexCoords);
void _managed_internal_MeshGetIndices(void* handle, MonoArray** outIndices);
void _managed_internal_MeshSetIndices(void* handle, MonoArray* inIndices);
void _managed_internal_ModelLoad(MonoString* path, MonoString* filepath, unsigned int* outID, void** outHandle);
void _managed_internal_ModelGetMeshes(void* handle, MonoArray** outMeshIDs, MonoArray** outMaterialIDs);
void _managed_internal_ShaderLoad0(MonoString* path, unsigned int* outResourceID, void** outHandle);
void _managed_internal_ShaderLoad1(MonoString* path, MonoString* vertexPath, MonoString* fragmentPath,  MonoString* computePath, MonoString* geometryPath,  unsigned int* outResourceID, void** outHandle);
void _managed_internal_ShaderBind(void* instance);
void _managed_internal_ShaderUnbind(void* instance);
void _managed_internal_ShaderUpdateStages(void* instance, MonoString* vertexPath, MonoString* fragmentPath,  MonoString* computePath, MonoString* geometryPath);
void _managed_internal_ShaderSet_int(void* instance, int location, int value);
void _managed_internal_ShaderSetStr_int(void* instance, MonoString* location, int value);
void _managed_internal_ShaderSet_bool(void* instance, int location, bool value);
void _managed_internal_ShaderSetStr_bool(void* instance, MonoString* location, bool value);
void _managed_internal_ShaderSet_float(void* instance, int location, float value);
void _managed_internal_ShaderSetStr_float(void* instance, MonoString* location, float value);
void _managed_internal_ShaderSet_double(void* instance, int location, double value);
void _managed_internal_ShaderSetStr_double(void* instance, MonoString* location, double value);
void _managed_internal_ShaderSet_vec2(void* instance, int location, glm::vec2* value);
void _managed_internal_ShaderSetStr_vec2(void* instance, MonoString* location, glm::vec2* value);
void _managed_internal_ShaderSet_vec3(void* instance, int location, glm::vec3* value);
void _managed_internal_ShaderSetStr_vec3(void* instance, MonoString* location, glm::vec3* value);
void _managed_internal_ShaderSet_vec4(void* instance, int location, glm::vec4* value);
void _managed_internal_ShaderSetStr_vec4(void* instance, MonoString* location, glm::vec4* value);
void _managed_internal_ShaderSet_mat3(void* instance, int location, glm::mat3* value);
void _managed_internal_ShaderSetStr_mat3(void* instance, MonoString* location, glm::mat3* value);
void _managed_internal_ShaderSet_mat4(void* instance, int location, glm::mat4* value);
void _managed_internal_ShaderSetStr_mat4(void* instance, MonoString* location, glm::mat4* value);
void _managed_internal_TextureLoad0(MonoString* path, unsigned int* outResourceID, void** outHandle);
void _managed_internal_TextureLoad1(MonoString* path, MonoString* filePath, bool hdr, unsigned int* outResourceID, void** outHandle);
void _managed_internal_TextureBind(void* instance, unsigned int index);
void _managed_internal_CameraSetMainCamera(void* handle);
void _managed_internal_CameraGetMainCamera(unsigned int* worldID, unsigned int* entityID);
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
void _managed_internal_LogNativeLog(MonoString* rawMsg, int level);
void _managed_internal_QuaternionToEuler(glm::quat* input, glm::vec3* output);
void _managed_internal_QuaternionFromEuler(glm::vec3* input, glm::quat* output);
void _managed_internal_QuaternionMultiply(glm::quat* a, glm::quat* b, glm::quat* output);
void _managed_internal_QuaternionMultiplyVector3(glm::quat* a, glm::vec3* b, glm::quat* output);
void _managed_internal_QuaternionInverse(glm::quat* input, glm::quat* output);
void _managed_internal_QuaternionInverseInline(glm::quat* input);
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
bool _managed_internal_SystemExists(unsigned int worldID, MonoReflectionType* componentType);
float _managed_internal_TimeFPS();
float _managed_internal_TimeDeltaTime();
float _managed_internal_TimeTimeSinceLaunch();
void _managed_internal_TransformGetPosition(void* handle, glm::vec3* output);
void _managed_internal_TransformSetPosition(void* handle, glm::vec3* input);
void _managed_internal_TransformGetRotation(void* handle, glm::quat* output);
void _managed_internal_TransformSetRotation(void* handle, glm::quat* input);
void _managed_internal_TransformGetScale(void* handle, glm::vec3* output);
void _managed_internal_TransformSetScale(void* handle, glm::vec3* input);
void _managed_internal_TransformGetUp(void* handle, glm::vec3* value);
void _managed_internal_TransformGetRight(void* handle, glm::vec3* value);
void _managed_internal_TransformGetForward(void* handle, glm::vec3* value);
float _managed_internal_Vector2Magnitude(glm::vec2* input);
float _managed_internal_Vector2Distance(glm::vec2* a, glm::vec2* b);
float _managed_internal_Vector3Distance(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Dot(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Magnitude(glm::vec3* input);
float _managed_internal_Vector4Magnitude(glm::vec4* input);
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
void _managed_internal_WindowSetResolution(glm::ivec2* value);
void _managed_internal_WindowGetResolution(glm::ivec2* outValue);
void _managed_internal_WindowSetFullscreenMode(int state);
int _managed_internal_WindowGetFullscreenMode();
unsigned int _managed_internal_ScreenGetVideoModes(MonoArray** outWidths, MonoArray** outHeights, MonoArray** outRefreshRates);
void _managed_internal_ScreenGetVideoMode(glm::ivec2* outResolution, float* outRefreshRate);
void _managed_internal_ScreenSetVideoMode(glm::ivec2* resolution, float refreshRate);
void _managed_internal_WindowCenterOnDisplay();
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


const vector<pair<const char*, const void*>> _InternalMethods = {
	MethodType("AquaEngine.Application::_Exit", (const void*)_managed_internal_ApplicationExit),
	MethodType("AquaEngine.Application::_GetArg", (const void*)_managed_internal_ApplicationGetArg),
	MethodType("AquaEngine.Application::_HasArg", (const void*)_managed_internal_ApplicationHasArg),
	MethodType("AquaEngine.Camera::_GetFar", (const void*)_managed_internal_CameraGetFar),
	MethodType("AquaEngine.Camera::_SetFar", (const void*)_managed_internal_CameraSetFar),
	MethodType("AquaEngine.Camera::_GetNear", (const void*)_managed_internal_CameraGetNear),
	MethodType("AquaEngine.Camera::_SetNear", (const void*)_managed_internal_CameraSetNear),
	MethodType("AquaEngine.Camera::_GetFieldOfView", (const void*)_managed_internal_CameraGetFieldOfView),
	MethodType("AquaEngine.Camera::_SetFieldOfView", (const void*)_managed_internal_CameraSetFieldOfView),
	MethodType("AquaEngine.Camera::_GetOrthographic", (const void*)_managed_internal_CameraGetOrthographic),
	MethodType("AquaEngine.Camera::_SetOrthographic", (const void*)_managed_internal_CameraSetOrthographic),
	MethodType("AquaEngine.Camera::_GetOrthographicSize", (const void*)_managed_internal_CameraGetOrthographicSize),
	MethodType("AquaEngine.Camera::_SetOrthographicSize", (const void*)_managed_internal_CameraSetOrthographicSize),
	MethodType("AquaEngine.NativeComponent::_GetHandle", (const void*)_managed_internal_NativeComponentGetHandle),
	MethodType("AquaEngine.MeshRenderer::_GetMesh", (const void*)_managed_internal_MeshRendererGetMesh),
	MethodType("AquaEngine.MeshRenderer::_SetMesh", (const void*)_managed_internal_MeshRendererSetMesh),
	MethodType("AquaEngine.MeshRenderer::_GetMaterial", (const void*)_managed_internal_MeshRendererGetMaterial),
	MethodType("AquaEngine.MeshRenderer::_SetMaterial", (const void*)_managed_internal_MeshRendererSetMaterial),
	MethodType("AquaEngine.Graphics.Material::_Load", (const void*)_managed_internal_MaterialLoad),
	MethodType("AquaEngine.Graphics.Material::_GetAlbedo", (const void*)_managed_internal_MaterialGetAlbedo),
	MethodType("AquaEngine.Graphics.Material::_SetAlbedo", (const void*)_managed_internal_MaterialSetAlbedo),
	MethodType("AquaEngine.Graphics.Material::_GetTextureCoordinateScale", (const void*)_managed_internal_MaterialGetTextureCoordinateScale),
	MethodType("AquaEngine.Graphics.Material::_SetTextureCoordinateScale", (const void*)_managed_internal_MaterialSetTextureCoordinateScale),
	MethodType("AquaEngine.Graphics.Material::_GetTextureCoordinateOffset", (const void*)_managed_internal_MaterialGetTextureCoordinateOffset),
	MethodType("AquaEngine.Graphics.Material::_SetTextureCoordinateOffset", (const void*)_managed_internal_MaterialSetTextureCoordinateOffset),
	MethodType("AquaEngine.Graphics.Material::_GetShader", (const void*)_managed_internal_MaterialGetShader),
	MethodType("AquaEngine.Graphics.Material::_SetShader", (const void*)_managed_internal_MaterialSetShader),
	MethodType("AquaEngine.Graphics.Material::_GetAlbedoMap", (const void*)_managed_internal_MaterialGetAlbedoMap),
	MethodType("AquaEngine.Graphics.Material::_SetAlbedoMap", (const void*)_managed_internal_MaterialSetAlbedoMap),
	MethodType("AquaEngine.Graphics.Material::_GetAlphaClipping", (const void*)_managed_internal_MaterialGetAlphaClipping),
	MethodType("AquaEngine.Graphics.Material::_SetAlphaClipping", (const void*)_managed_internal_MaterialSetAlphaClipping),
	MethodType("AquaEngine.Graphics.Material::_GetAlphaClipThreshold", (const void*)_managed_internal_MaterialGetAlphaClipThreshold),
	MethodType("AquaEngine.Graphics.Material::_SetAlphaClipThreshold", (const void*)_managed_internal_MaterialSetAlphaClipThreshold),
	MethodType("AquaEngine.Graphics.Material::_GetRoughness", (const void*)_managed_internal_MaterialGetRoughness),
	MethodType("AquaEngine.Graphics.Material::_SetRoughness", (const void*)_managed_internal_MaterialSetRoughness),
	MethodType("AquaEngine.Graphics.Material::_GetRoughnessMap", (const void*)_managed_internal_MaterialGetRoughnessMap),
	MethodType("AquaEngine.Graphics.Material::_SetRoughnessMap", (const void*)_managed_internal_MaterialSetRoughnessMap),
	MethodType("AquaEngine.Graphics.Material::_GetMetalness", (const void*)_managed_internal_MaterialGetMetalness),
	MethodType("AquaEngine.Graphics.Material::_SetMetalness", (const void*)_managed_internal_MaterialSetMetalness),
	MethodType("AquaEngine.Graphics.Material::_GetMetalnessMap", (const void*)_managed_internal_MaterialGetMetalnessMap),
	MethodType("AquaEngine.Graphics.Material::_SetMetalnessMap", (const void*)_managed_internal_MaterialSetMetalnessMap),
	MethodType("AquaEngine.Graphics.Material::_GetNormalMap", (const void*)_managed_internal_MaterialGetNormalMap),
	MethodType("AquaEngine.Graphics.Material::_SetNormalMap", (const void*)_managed_internal_MaterialSetNormalMap),
	MethodType("AquaEngine.Graphics.Material::_GetAmbientOcclusionMap", (const void*)_managed_internal_MaterialGetAmbientOcclusionMap),
	MethodType("AquaEngine.Graphics.Material::_SetAmbientOcclusionMap", (const void*)_managed_internal_MaterialSetAmbientOcclusionMap),
	MethodType("AquaEngine.Graphics.Material::_GetTransparent", (const void*)_managed_internal_MaterialGetTransparent),
	MethodType("AquaEngine.Graphics.Material::_SetTransparent", (const void*)_managed_internal_MaterialSetTransparent),
	MethodType("AquaEngine.Graphics.Mesh::_Load0", (const void*)_managed_internal_MeshLoad0),
	MethodType("AquaEngine.Graphics.Mesh::_Load1", (const void*)_managed_internal_MeshLoad1),
	MethodType("AquaEngine.Graphics.Mesh::_GetVertices", (const void*)_managed_internal_MeshGetVertices),
	MethodType("AquaEngine.Graphics.Mesh::_SetVertices", (const void*)_managed_internal_MeshSetVertices),
	MethodType("AquaEngine.Graphics.Mesh::_GetIndices", (const void*)_managed_internal_MeshGetIndices),
	MethodType("AquaEngine.Graphics.Mesh::_SetIndices", (const void*)_managed_internal_MeshSetIndices),
	MethodType("AquaEngine.Graphics.Model::_Load", (const void*)_managed_internal_ModelLoad),
	MethodType("AquaEngine.Graphics.Model::_GetMeshes", (const void*)_managed_internal_ModelGetMeshes),
	MethodType("AquaEngine.Graphics.Shader::_Load0", (const void*)_managed_internal_ShaderLoad0),
	MethodType("AquaEngine.Graphics.Shader::_Load1", (const void*)_managed_internal_ShaderLoad1),
	MethodType("AquaEngine.Graphics.Shader::_Bind", (const void*)_managed_internal_ShaderBind),
	MethodType("AquaEngine.Graphics.Shader::_Unbind", (const void*)_managed_internal_ShaderUnbind),
	MethodType("AquaEngine.Graphics.Shader::_UpdateStages", (const void*)_managed_internal_ShaderUpdateStages),
	MethodType("AquaEngine.Graphics.Shader::_Set_int", (const void*)_managed_internal_ShaderSet_int),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_int", (const void*)_managed_internal_ShaderSetStr_int),
	MethodType("AquaEngine.Graphics.Shader::_Set_bool", (const void*)_managed_internal_ShaderSet_bool),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_bool", (const void*)_managed_internal_ShaderSetStr_bool),
	MethodType("AquaEngine.Graphics.Shader::_Set_float", (const void*)_managed_internal_ShaderSet_float),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_float", (const void*)_managed_internal_ShaderSetStr_float),
	MethodType("AquaEngine.Graphics.Shader::_Set_double", (const void*)_managed_internal_ShaderSet_double),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_double", (const void*)_managed_internal_ShaderSetStr_double),
	MethodType("AquaEngine.Graphics.Shader::_Set_vec2", (const void*)_managed_internal_ShaderSet_vec2),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_vec2", (const void*)_managed_internal_ShaderSetStr_vec2),
	MethodType("AquaEngine.Graphics.Shader::_Set_vec3", (const void*)_managed_internal_ShaderSet_vec3),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_vec3", (const void*)_managed_internal_ShaderSetStr_vec3),
	MethodType("AquaEngine.Graphics.Shader::_Set_vec4", (const void*)_managed_internal_ShaderSet_vec4),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_vec4", (const void*)_managed_internal_ShaderSetStr_vec4),
	MethodType("AquaEngine.Graphics.Shader::_Set_mat3", (const void*)_managed_internal_ShaderSet_mat3),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_mat3", (const void*)_managed_internal_ShaderSetStr_mat3),
	MethodType("AquaEngine.Graphics.Shader::_Set_mat4", (const void*)_managed_internal_ShaderSet_mat4),
	MethodType("AquaEngine.Graphics.Shader::_SetStr_mat4", (const void*)_managed_internal_ShaderSetStr_mat4),
	MethodType("AquaEngine.Graphics.Texture::_Load0", (const void*)_managed_internal_TextureLoad0),
	MethodType("AquaEngine.Graphics.Texture::_Load1", (const void*)_managed_internal_TextureLoad1),
	MethodType("AquaEngine.Graphics.Texture::_Bind", (const void*)_managed_internal_TextureBind),
	MethodType("AquaEngine.Camera::_SetMainCamera", (const void*)_managed_internal_CameraSetMainCamera),
	MethodType("AquaEngine.Camera::_GetMainCamera", (const void*)_managed_internal_CameraGetMainCamera),
	MethodType("AquaEngine.Input::_InputEnabled", (const void*)_managed_internal_InputInputEnabled),
	MethodType("AquaEngine.Input::_InputEnable", (const void*)_managed_internal_InputInputEnable),
	MethodType("AquaEngine.Input::_IsKeyUp", (const void*)_managed_internal_InputIsKeyUp),
	MethodType("AquaEngine.Input::_IsKeyDown", (const void*)_managed_internal_InputIsKeyDown),
	MethodType("AquaEngine.Input::_IsKeyPressed", (const void*)_managed_internal_InputIsKeyPressed),
	MethodType("AquaEngine.Input::_IsKeyReleased", (const void*)_managed_internal_InputIsKeyReleased),
	MethodType("AquaEngine.Input::_ScrollDelta", (const void*)_managed_internal_InputScrollDelta),
	MethodType("AquaEngine.Input::_SetMousePosition", (const void*)_managed_internal_InputSetMousePosition),
	MethodType("AquaEngine.Input::_GetMouseDelta", (const void*)_managed_internal_InputGetMouseDelta),
	MethodType("AquaEngine.Input::_GetMousePosition", (const void*)_managed_internal_InputGetMousePosition),
	MethodType("AquaEngine.Input::_IsMouseUp", (const void*)_managed_internal_InputIsMouseUp),
	MethodType("AquaEngine.Input::_IsMouseDown", (const void*)_managed_internal_InputIsMouseDown),
	MethodType("AquaEngine.Input::_IsMousePressed", (const void*)_managed_internal_InputIsMousePressed),
	MethodType("AquaEngine.Input::_IsMouseReleased", (const void*)_managed_internal_InputIsMouseReleased),
	MethodType("AquaEngine.Input::_IsMouseShowing", (const void*)_managed_internal_InputIsMouseShowing),
	MethodType("AquaEngine.Input::_GetMouseState", (const void*)_managed_internal_InputGetMouseState),
	MethodType("AquaEngine.Input::_SetMouseState", (const void*)_managed_internal_InputSetMouseState),
	MethodType("AquaEngine.Input::_IsGamepadConnected", (const void*)_managed_internal_InputIsGamepadConnected),
	MethodType("AquaEngine.Input::_GetAxis", (const void*)_managed_internal_InputGetAxis),
	MethodType("AquaEngine.Input::_IsButtonUp", (const void*)_managed_internal_InputIsButtonUp),
	MethodType("AquaEngine.Input::_IsButtonDown", (const void*)_managed_internal_InputIsButtonDown),
	MethodType("AquaEngine.Log::_NativeLog", (const void*)_managed_internal_LogNativeLog),
	MethodType("AquaEngine.Quaternion::_ToEuler", (const void*)_managed_internal_QuaternionToEuler),
	MethodType("AquaEngine.Quaternion::_FromEuler", (const void*)_managed_internal_QuaternionFromEuler),
	MethodType("AquaEngine.Quaternion::_Multiply", (const void*)_managed_internal_QuaternionMultiply),
	MethodType("AquaEngine.Quaternion::_MultiplyVector3", (const void*)_managed_internal_QuaternionMultiplyVector3),
	MethodType("AquaEngine.Quaternion::_Inverse", (const void*)_managed_internal_QuaternionInverse),
	MethodType("AquaEngine.Quaternion::_InverseInline", (const void*)_managed_internal_QuaternionInverseInline),
	MethodType("AquaEngine.Resource::_Duplicate", (const void*)_managed_internal_ResourceDuplicate),
	MethodType("AquaEngine.Resource::_GetPath", (const void*)_managed_internal_ResourceGetPath),
	MethodType("AquaEngine.Resource::_GetID", (const void*)_managed_internal_ResourceGetID),
	MethodType("AquaEngine.Resource::_Unload", (const void*)_managed_internal_ResourceUnload),
	MethodType("AquaEngine.Resource::_GetInstance", (const void*)_managed_internal_ResourceGetInstance),
	MethodType("AquaEngine.Resource::_Exists", (const void*)_managed_internal_ResourceExists),
	MethodType("AquaEngine.SpriteRenderer::_GetSprite", (const void*)_managed_internal_SpriteRendererGetSprite),
	MethodType("AquaEngine.SpriteRenderer::_SetSprite", (const void*)_managed_internal_SpriteRendererSetSprite),
	MethodType("AquaEngine.SpriteRenderer::_GetShader", (const void*)_managed_internal_SpriteRendererGetShader),
	MethodType("AquaEngine.SpriteRenderer::_SetShader", (const void*)_managed_internal_SpriteRendererSetShader),
	MethodType("AquaEngine.SpriteRenderer::_GetColour", (const void*)_managed_internal_SpriteRendererGetColour),
	MethodType("AquaEngine.SpriteRenderer::_SetColour", (const void*)_managed_internal_SpriteRendererSetColour),
	MethodType("AquaEngine.System::_Exists", (const void*)_managed_internal_SystemExists),
	MethodType("AquaEngine.Time::_FPS", (const void*)_managed_internal_TimeFPS),
	MethodType("AquaEngine.Time::_DeltaTime", (const void*)_managed_internal_TimeDeltaTime),
	MethodType("AquaEngine.Time::_TimeSinceLaunch", (const void*)_managed_internal_TimeTimeSinceLaunch),
	MethodType("AquaEngine.Transform::_GetPosition", (const void*)_managed_internal_TransformGetPosition),
	MethodType("AquaEngine.Transform::_SetPosition", (const void*)_managed_internal_TransformSetPosition),
	MethodType("AquaEngine.Transform::_GetRotation", (const void*)_managed_internal_TransformGetRotation),
	MethodType("AquaEngine.Transform::_SetRotation", (const void*)_managed_internal_TransformSetRotation),
	MethodType("AquaEngine.Transform::_GetScale", (const void*)_managed_internal_TransformGetScale),
	MethodType("AquaEngine.Transform::_SetScale", (const void*)_managed_internal_TransformSetScale),
	MethodType("AquaEngine.Transform::_GetUp", (const void*)_managed_internal_TransformGetUp),
	MethodType("AquaEngine.Transform::_GetRight", (const void*)_managed_internal_TransformGetRight),
	MethodType("AquaEngine.Transform::_GetForward", (const void*)_managed_internal_TransformGetForward),
	MethodType("AquaEngine.Vector2::_Magnitude", (const void*)_managed_internal_Vector2Magnitude),
	MethodType("AquaEngine.Vector2::_Distance", (const void*)_managed_internal_Vector2Distance),
	MethodType("AquaEngine.Vector3::_Distance", (const void*)_managed_internal_Vector3Distance),
	MethodType("AquaEngine.Vector3::_Dot", (const void*)_managed_internal_Vector3Dot),
	MethodType("AquaEngine.Vector3::_Magnitude", (const void*)_managed_internal_Vector3Magnitude),
	MethodType("AquaEngine.Vector4::_Magnitude", (const void*)_managed_internal_Vector4Magnitude),
	MethodType("AquaEngine.VFS::_GetCurrentDirectory", (const void*)_managed_internal_VFSGetCurrentDirectory),
	MethodType("AquaEngine.VFS::_HasMount", (const void*)_managed_internal_VFSHasMount),
	MethodType("AquaEngine.VFS::_Mount", (const void*)_managed_internal_VFSMount),
	MethodType("AquaEngine.VFS::_MountPath", (const void*)_managed_internal_VFSMountPath),
	MethodType("AquaEngine.VFS::_Unmount", (const void*)_managed_internal_VFSUnmount),
	MethodType("AquaEngine.VFS::_UnmountPath", (const void*)_managed_internal_VFSUnmountPath),
	MethodType("AquaEngine.VFS::_ReadText", (const void*)_managed_internal_VFSReadText),
	MethodType("AquaEngine.VFS::_WriteText", (const void*)_managed_internal_VFSWriteText),
	MethodType("AquaEngine.VFS::_Read", (const void*)_managed_internal_VFSRead),
	MethodType("AquaEngine.VFS::_Write", (const void*)_managed_internal_VFSWrite),
	MethodType("AquaEngine.VFS::_Remove", (const void*)_managed_internal_VFSRemove),
	MethodType("AquaEngine.VFS::_Move", (const void*)_managed_internal_VFSMove),
	MethodType("AquaEngine.VFS::_Copy", (const void*)_managed_internal_VFSCopy),
	MethodType("AquaEngine.VFS::_GetAbsolutePath", (const void*)_managed_internal_VFSGetAbsolutePath),
	MethodType("AquaEngine.VFS::_Exists", (const void*)_managed_internal_VFSExists),
	MethodType("AquaEngine.Window::_SetResolution", (const void*)_managed_internal_WindowSetResolution),
	MethodType("AquaEngine.Window::_GetResolution", (const void*)_managed_internal_WindowGetResolution),
	MethodType("AquaEngine.Window::_SetFullscreenMode", (const void*)_managed_internal_WindowSetFullscreenMode),
	MethodType("AquaEngine.Window::_GetFullscreenMode", (const void*)_managed_internal_WindowGetFullscreenMode),
	MethodType("AquaEngine.Screen::_GetVideoModes", (const void*)_managed_internal_ScreenGetVideoModes),
	MethodType("AquaEngine.Screen::_GetVideoMode", (const void*)_managed_internal_ScreenGetVideoMode),
	MethodType("AquaEngine.Screen::_SetVideoMode", (const void*)_managed_internal_ScreenSetVideoMode),
	MethodType("AquaEngine.Window::_CenterOnDisplay", (const void*)_managed_internal_WindowCenterOnDisplay),
	MethodType("AquaEngine.World::_Get", (const void*)_managed_internal_WorldGet),
	MethodType("AquaEngine.World::_Exists", (const void*)_managed_internal_WorldExists),
	MethodType("AquaEngine.World::_Destroy", (const void*)_managed_internal_WorldDestroy),
	MethodType("AquaEngine.World::_Create", (const void*)_managed_internal_WorldCreate),
	MethodType("AquaEngine.World::_CreateEntity", (const void*)_managed_internal_WorldCreateEntity),
	MethodType("AquaEngine.World::_DestroyEntity", (const void*)_managed_internal_WorldDestroyEntity),
	MethodType("AquaEngine.World::_HasEntity", (const void*)_managed_internal_WorldHasEntity),
	MethodType("AquaEngine.World::_GetEntities", (const void*)_managed_internal_WorldGetEntities),
	MethodType("AquaEngine.World::_GetComponents", (const void*)_managed_internal_WorldGetComponents),
	MethodType("AquaEngine.World::_GetComponentsMultiple", (const void*)_managed_internal_WorldGetComponentsMultiple),
	MethodType("AquaEngine.Entity::_HasComponent", (const void*)_managed_internal_EntityHasComponent),
	MethodType("AquaEngine.Entity::_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	MethodType("AquaEngine.Entity::_AddComponent", (const void*)_managed_internal_EntityAddComponent),
	MethodType("AquaEngine.Entity::_RemoveComponent", (const void*)_managed_internal_EntityRemoveComponent),
	MethodType("AquaEngine.Entity::_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	MethodType("AquaEngine.World::_HasSystem", (const void*)_managed_internal_WorldHasSystem),
	MethodType("AquaEngine.World::_GetSystem", (const void*)_managed_internal_WorldGetSystem),
	MethodType("AquaEngine.World::_AddSystem", (const void*)_managed_internal_WorldAddSystem),
	MethodType("AquaEngine.World::_RemoveSystem", (const void*)_managed_internal_WorldRemoveSystem)
};