#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

using namespace std; 

typedef pair<const char*, const void*> MethodType; 

void _managed_internal_ApplicationExit();
MonoString* _managed_internal_ApplicationGetArg(MonoString* name, MonoString* defaultValue);
MonoString* _managed_internal_ApplicationGetExecutablePath();
MonoString* _managed_internal_ApplicationGetExecutableDirectory();
bool _managed_internal_ApplicationHasArg(MonoString* name);
unsigned char _managed_internal_ApplicationGetBuildType();
unsigned char _managed_internal_ApplicationGetPlatform();
bool _managed_internal_ApplicationIsDesktop();
void _managed_internal_SoundLoad(MonoString* path, unsigned long long* outResourceID, void** outHandle);
void _managed_internal_SoundImport(void* handle, MonoString* path);
void _managed_internal_SoundMixerLoad(MonoString* pathRaw, uint64_t* outResourceID, void** outHandle);
MonoString* _managed_internal_SoundMixerGetName(void* handle);
void _managed_internal_SoundMixerSetName(void* handle, MonoString* value);
float _managed_internal_SoundMixerGetVolume(void* handle);
void _managed_internal_SoundMixerSetVolume(void* handle, float value);
uint64_t _managed_internal_SoundMixerGetParent(void* handle);
void _managed_internal_SoundMixerSetParent(void* handle, uint64_t value);
void _managed_internal_CameraSetRenderTarget(void* handle, void* renderTarget);
float _managed_internal_CameraGetFar(void* handle);
void _managed_internal_CameraSetFar(void* handle, float value);
float _managed_internal_CameraGetNear(void* handle);
void _managed_internal_CameraSetNear(void* handle, float value);
void* _managed_internal_CameraGetRenderTarget(void* handle);
float _managed_internal_CameraGetFieldOfView(void* handle);
void _managed_internal_CameraSetFieldOfView(void* handle, float value);
bool _managed_internal_CameraGetOrthographic(void* handle);
void _managed_internal_CameraSetOrthographic(void* handle, bool value);
float _managed_internal_CameraGetOrthographicSize(void* handle);
void _managed_internal_CameraSetOrthographicSize(void* handle, float value);
uint64_t _managed_internal_MeshRendererGetMesh(void* handle);
void _managed_internal_MeshRendererSetMesh(void* handle, uint64_t value);
uint64_t _managed_internal_MeshRendererGetMaterial(void* handle);
void _managed_internal_MeshRendererSetMaterial(void* handle, uint64_t value);
uint64_t _managed_internal_SoundSourceGetSound(void* instance);
void _managed_internal_SoundSourceSetSound(void* instance, uint64_t value);
void _managed_internal_SoundSourcePlay(void* instance);
void _managed_internal_SoundSourcePause(void* instance);
void _managed_internal_SoundSourceStop(void* instance);
void _managed_internal_SoundSourceSeek(void* instance, float seconds);
bool _managed_internal_SoundSourceIsPlaying(void* instance);
unsigned int _managed_internal_SoundSourceGetState(void* instance);
float _managed_internal_SoundSourceGetPlayTime(void* instance);
float _managed_internal_SoundSourceGetLength(void* instance);
bool _managed_internal_SoundSourceGetLooping(void* instance);
void _managed_internal_SoundSourceSetLooping(void* instance, bool value);
bool _managed_internal_SoundSourceGetSpatialization(void* instance);
void _managed_internal_SoundSourceSetSpatialization(void* instance, bool value);
uint64_t _managed_internal_SoundSourceGetMixer(void* instance);
void _managed_internal_SoundSourceSetMixer(void* instance, uint64_t value);
float _managed_internal_SoundSourceGetPanning(void* instance);
void _managed_internal_SoundSourceSetPanning(void* instance, float value);
float _managed_internal_SoundSourceGetVolume(void* instance);
void _managed_internal_SoundSourceSetVolume(void* instance, float value);
float _managed_internal_SoundSourceGetPitch(void* instance);
void _managed_internal_SoundSourceSetPitch(void* instance, float value);
void _managed_internal_TransformGetPosition(void* handle, glm::vec3* output);
void _managed_internal_TransformSetPosition(void* handle, glm::vec3* input);
void _managed_internal_TransformGetGlobalPosition(void* handle, glm::vec3* output);
void _managed_internal_TransformGetRotation(void* handle, glm::quat* output);
void _managed_internal_TransformSetRotation(void* handle, glm::quat* input);
void _managed_internal_TransformGetGlobalRotation(void* handle, glm::quat* output);
void _managed_internal_TransformGetScale(void* handle, glm::vec3* output);
void _managed_internal_TransformSetScale(void* handle, glm::vec3* input);
void _managed_internal_TransformGetGlobalScale(void* handle, glm::vec3* output);
void _managed_internal_TransformGetUp(void* handle, glm::vec3* value);
void _managed_internal_TransformGetRight(void* handle, glm::vec3* value);
void _managed_internal_TransformGetForward(void* handle, glm::vec3* value);
MonoObject* _managed_internal_TransformGetParent(void* handle);
void _managed_internal_TransformSetParent(void* handle, void* parentHandle);
MonoArray* _managed_internal_TransformGetChildren(void* handle);
void _managed_internal_TransformAddChild(void* handle, void* childHandle);
void _managed_internal_TransformRemoveChild(void* handle, void* childHandle);
void* _managed_internal_FramebufferLoad(glm::ivec2* resolution, bool swapchainTarget, unsigned int samples, MonoArray* attachments);
void _managed_internal_FramebufferDestroy(void* handle);
void _managed_internal_FramebufferBind(void* handle);
void _managed_internal_FramebufferUnind(void* handle);
void _managed_internal_FramebufferSetSamples(void* handle, unsigned int samples);
void _managed_internal_FramebufferSetResolution(void* handle, glm::ivec2* resolution);
unsigned int _managed_internal_FramebufferGetAttachmentCount(void* handle);
unsigned int _managed_internal_FramebufferGetColourAttachmentCount(void* handle);
void _managed_internal_FramebufferCopyAttachmentTo(void* handle, void* destination, unsigned int attachmentIndex);
void _managed_internal_FramebufferBlitTo(void* handle, void* destination, unsigned int flags);
MonoArray* _managed_internal_FramebufferGetAttachments(void* handle, int* outDepthAttachmentIndex);
void _managed_internal_MaterialLoad(MonoString* pathRaw, uint64_t* resourceID, void** outHandle);
void _managed_internal_MaterialGetAlbedo(void* handle, glm::vec4* value);
void _managed_internal_MaterialSetAlbedo(void* handle, glm::vec4* value);
void _managed_internal_MaterialGetTextureCoordinateScale(void* handle, glm::vec2* value);
void _managed_internal_MaterialSetTextureCoordinateScale(void* handle, glm::vec2* value);
void _managed_internal_MaterialGetTextureCoordinateOffset(void* handle, glm::vec2* value);
void _managed_internal_MaterialSetTextureCoordinateOffset(void* handle, glm::vec2* value);
uint64_t _managed_internal_MaterialGetShader(void* handle);
void _managed_internal_MaterialSetShader(void* handle, uint64_t value);
uint64_t _managed_internal_MaterialGetAlbedoMap(void* handle);
void _managed_internal_MaterialSetAlbedoMap(void* handle, uint64_t value);
bool _managed_internal_MaterialGetAlphaClipping(void* handle);
void _managed_internal_MaterialSetAlphaClipping(void* handle, bool value);
float _managed_internal_MaterialGetAlphaClipThreshold(void* handle);
void _managed_internal_MaterialSetAlphaClipThreshold(void* handle, float value);
float _managed_internal_MaterialGetRoughness(void* handle);
void _managed_internal_MaterialSetRoughness(void* handle, float value);
uint64_t _managed_internal_MaterialGetRoughnessMap(void* handle);
void _managed_internal_MaterialSetRoughnessMap(void* handle, uint64_t value);
float _managed_internal_MaterialGetMetalness(void* handle);
void _managed_internal_MaterialSetMetalness(void* handle, float value);
uint64_t _managed_internal_MaterialGetMetalnessMap(void* handle);
void _managed_internal_MaterialSetMetalnessMap(void* handle, uint64_t value);
uint64_t _managed_internal_MaterialGetNormalMap(void* handle);
void _managed_internal_MaterialSetNormalMap(void* handle, uint64_t value);
uint64_t _managed_internal_MaterialGetAmbientOcclusionMap(void* handle);
void _managed_internal_MaterialSetAmbientOcclusionMap(void* handle, uint64_t value);
bool _managed_internal_MaterialGetTransparent(void* handle);
void _managed_internal_MaterialSetTransparent(void* handle, bool value);
void _managed_internal_MeshLoad(MonoString* pathRaw, uint64_t* outID, void** outHandle);
void _managed_internal_MeshImport(void* handle, MonoArray* inVertices, MonoArray* inIndices, unsigned char drawMode);
void _managed_internal_MeshGetVertices(void* handle, MonoArray** outPositions, MonoArray** outNormals, MonoArray** outTexCoords);
void _managed_internal_MeshSetVertices(void* handle, MonoArray* inPositions, MonoArray* inNormals, MonoArray* inTexCoords);
void _managed_internal_MeshGetIndices(void* handle, MonoArray** outIndices);
void _managed_internal_MeshSetIndices(void* handle, MonoArray* inIndices);
void _managed_internal_ModelLoad(MonoString* pathRaw, uint64_t* outID, void** outHandle);
void _managed_internal_ModelImport(void* handle, MonoString* filepath, MonoArray* modelData, bool importMaterials);
void _managed_internal_ModelGetMeshes(void* handle, MonoArray** outMeshIDs, MonoArray** outMaterialIDs);
void _managed_internal_NativeRenderPipelineDraw(void* handle, void* cameraHandle);
void _managed_internal_NativeRenderPipelineSetResolution(void* handle, glm::ivec2* resolution);
void _managed_internal_NativeRenderPipelineGetResolution(void* handle, glm::ivec2* resolution);
void* _managed_internal_NativeRenderPipelineGetOutput(void* handle);
void* _managed_internal_RenderTextureLoad(glm::ivec2* resolution, unsigned char textureFormat, unsigned int samples);
void _managed_internal_RenderTextureDestroy(void* handle);
void _managed_internal_RenderTextureBind(void* handle, unsigned int index);
void _managed_internal_RenderTextureUnbind(void* handle);
void _managed_internal_RenderTextureSetFormat(void* handle, unsigned char format);
void _managed_internal_RenderTextureSetSamples(void* handle, unsigned int samples);
void _managed_internal_RenderTextureSetResolution(void* handle, glm::ivec2* resolution);
unsigned char _managed_internal_RenderTextureGetFormat(void* handle);
unsigned int _managed_internal_RenderTextureGetSamples(void* handle);
void _managed_internal_RenderTextureGetResolution(void* handle, glm::ivec2* resolution);
void _managed_internal_RenderTextureCopyTo(void* handle, void* destination);
void _managed_internal_ShaderLoad(MonoString* path, uint64_t* outResourceID, void** outHandle);
void _managed_internal_ShaderBind(void* instance);
void _managed_internal_ShaderUnbind(void* instance);
void _managed_internal_ShaderUpdateStages(void* instance, MonoString* vertexPath, MonoString* vertexContents,  MonoString* fragmentPath, MonoString* fragmentContents,  MonoString* computePath, MonoString* computeContents,  MonoString* geometryPath, MonoString* geometryContents);
void _managed_internal_ShaderUnloadStages(void* instance);
void _managed_internal_ShaderGetStages(void* instance, MonoString** outVertex, MonoString** outFragment, MonoString** outCompute, MonoString** outGeometry);
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
void _managed_internal_TextureLoad(MonoString* pathRaw, uint64_t* outResourceID, void** outHandle);
bool _managed_internal_TextureUpload(void* instance, MonoArray* textureDataRaw, bool hdr, int filter);
void _managed_internal_TextureBind(void* instance, unsigned int index);
void _managed_internal_TextureGetResolution(void* instance, glm::ivec2* outResolution);
bool _managed_internal_TextureGetHDR(void* instance);
int _managed_internal_TextureGetFilter(void* instance);
bool _managed_internal_TextureIsValid(void* instance);
void _managed_internal_ClipboardSetText(MonoString* value);
MonoString* _managed_internal_ClipboardGetText();
unsigned int _managed_internal_AudioGetDeviceCount();
unsigned int _managed_internal_AudioGetDefaultDevice();
MonoString* _managed_internal_AudioGetDeviceName(unsigned int index);
void _managed_internal_AudioSetOutputDevice(unsigned int index);
unsigned int _managed_internal_AudioGetOutputDevice();
bool _managed_internal_BaseGameLauncherLoadAssembly(MonoArray* dataRaw, MonoString* friendlyNameRaw);
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
void _managed_internal_QuaternionMultiplyVector3(glm::quat* a, glm::vec3* b, glm::vec3* output);
void _managed_internal_QuaternionInverse(glm::quat* input, glm::quat* output);
void _managed_internal_QuaternionInverseInline(glm::quat* input);
void _managed_internal_QuaternionAngleAxis(float angle, glm::vec3* axis, glm::quat* output);
void _managed_internal_QuaternionNormalise(glm::quat* value);
void _managed_internal_RandomSeed(unsigned int seed);
int _managed_internal_RandomInt(int min, int max);
float _managed_internal_RandomFloat(float min, float max);
uint64_t _managed_internal_ResourceDuplicate(uint64_t originalID, MonoString* newPath);
MonoString* _managed_internal_ResourceGetPath(uint64_t id);
uint64_t _managed_internal_ResourceGetID(MonoString* path);
uint64_t _managed_internal_ResourceCreateID();
void _managed_internal_ResourceUnload(uint64_t resourceID);
void* _managed_internal_ResourceGetInstance(uint64_t resourceID);
bool _managed_internal_ResourceExists(uint64_t resourceID);
void _managed_internal_ResourcePrint();
void _managed_internal_SpriteRendererGetColour(void* handle, glm::vec4* colour);
void _managed_internal_SpriteRendererSetColour(void* handle, glm::vec4* colour);
uint64_t _managed_internal_SpriteRendererGetSprite(void* handle);
void _managed_internal_SpriteRendererSetSprite(void* handle, uint64_t value);
uint64_t _managed_internal_SpriteRendererGetShader(void* handle);
void _managed_internal_SpriteRendererSetShader(void* handle, uint64_t value);
float _managed_internal_TimeFPS();
float _managed_internal_TimeDeltaTime();
float _managed_internal_TimeTimeSinceLaunch();
float _managed_internal_Vector2Magnitude(glm::vec2* input);
float _managed_internal_Vector2Distance(glm::vec2* a, glm::vec2* b);
float _managed_internal_Vector3Distance(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Dot(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Magnitude(glm::vec3* input);
float _managed_internal_Vector4Magnitude(glm::vec4* input);
void _managed_internal_WindowSetResolution(glm::ivec2* value);
void _managed_internal_WindowGetResolution(glm::ivec2* outValue);
void _managed_internal_WindowSetFullscreenMode(int state);
int _managed_internal_WindowGetFullscreenMode();
void _managed_internal_WindowGetContentScaling(glm::vec2* outValue);
unsigned int _managed_internal_ScreenGetVideoModes(MonoArray** outWidths, MonoArray** outHeights, MonoArray** outRefreshRates);
void _managed_internal_ScreenGetVideoMode(glm::ivec2* outResolution, float* outRefreshRate);
void _managed_internal_ScreenSetVideoMode(glm::ivec2* resolution, float refreshRate);
void _managed_internal_WindowCenterOnDisplay();
MonoString* _managed_internal_WindowGetTitle();
void _managed_internal_WindowSetTitle(MonoString* titleRaw);
bool _managed_internal_WindowRequestedToClose();
bool _managed_internal_WindowGetVSync();
void _managed_internal_WindowSetVSync(bool value);
bool _managed_internal_WorldGet(uint64_t worldID, MonoString** outName);
void _managed_internal_WorldGetAll(MonoArray** outWorldIDs);
bool _managed_internal_WorldExists(uint64_t worldID);
bool _managed_internal_WorldDestroy(uint64_t worldID);
uint64_t _managed_internal_WorldCreate(MonoString* nameRaw);
void _managed_internal_WorldSetName(uint64_t worldID, MonoString* nameRaw);
uint64_t _managed_internal_WorldCreateEntity(uint64_t worldID);
uint64_t _managed_internal_WorldCreateEntityID(uint64_t worldID, uint64_t entityID);
void _managed_internal_WorldDestroyEntity(uint64_t worldID, uint64_t entityID);
bool _managed_internal_WorldHasEntity(uint64_t worldID, uint64_t entityID);
MonoArray* _managed_internal_WorldGetEntities(uint64_t worldID);
MonoArray* _managed_internal_WorldGetComponents(uint64_t worldID, MonoReflectionType* componentType);
MonoArray* _managed_internal_WorldGetComponentsMultiple(uint64_t worldID, MonoArray* inputTypes);
bool _managed_internal_EntityHasComponent(uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType);
MonoObject* _managed_internal_EntityGetComponent(uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType);
MonoObject* _managed_internal_EntityAddComponent(uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType, void** outHandle);
bool _managed_internal_EntityRemoveComponent(uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType);
MonoObject* _managed_internal_EntityGetComponent(uint64_t worldID, uint64_t entityID, MonoReflectionType* componentType);
MonoArray* _managed_internal_EntityGetComponents(uint64_t worldID, uint64_t entityID);
bool _managed_internal_WorldHasSystem(uint64_t worldID, MonoReflectionType* systemType);
MonoObject* _managed_internal_WorldGetSystem(uint64_t worldID, MonoReflectionType* systemType);
MonoArray* _managed_internal_WorldGetSystems(uint64_t worldID);
MonoObject* _managed_internal_WorldAddSystem(uint64_t worldID, MonoReflectionType* systemType);
bool _managed_internal_WorldRemoveSystem(uint64_t worldID, MonoReflectionType* systemType);
void _managed_internal_WorldEnableSystem(uint64_t worldID, MonoReflectionType* systemType, bool enable);
void* _managed_internal_RendererGetPipeline();
void _managed_internal_RendererDraw();
void _managed_internal_RendererDrawFromCamera(unsigned int worldID, unsigned int entityID);
void _managed_internal_SceneManagerLoad(uint64_t worldID);
void _managed_internal_SceneManagerLoadAdditive(uint64_t worldID);
void _managed_internal_SceneManagerUnload(uint64_t worldID);
void _managed_internal_SceneManagerUnloadAll();
MonoArray* _managed_internal_SceneManagerGetActiveScenes();
void* _managed_internal_NativeSystemGetHandle(unsigned int worldID, MonoReflectionType* type);


const vector<pair<const char*, const void*>> _InternalMethods = {
	MethodType("Yonai.Application::_Exit", (const void*)_managed_internal_ApplicationExit),
	MethodType("Yonai.Application::_GetArg", (const void*)_managed_internal_ApplicationGetArg),
	MethodType("Yonai.Application::_GetExecutablePath", (const void*)_managed_internal_ApplicationGetExecutablePath),
	MethodType("Yonai.Application::_GetExecutableDirectory", (const void*)_managed_internal_ApplicationGetExecutableDirectory),
	MethodType("Yonai.Application::_HasArg", (const void*)_managed_internal_ApplicationHasArg),
	MethodType("Yonai.Application::_GetBuildType", (const void*)_managed_internal_ApplicationGetBuildType),
	MethodType("Yonai.Application::_GetPlatform", (const void*)_managed_internal_ApplicationGetPlatform),
	MethodType("Yonai.Application::_IsDesktop", (const void*)_managed_internal_ApplicationIsDesktop),
	MethodType("Yonai.Sound::_Load", (const void*)_managed_internal_SoundLoad),
	MethodType("Yonai.Sound::_Import", (const void*)_managed_internal_SoundImport),
	MethodType("Yonai.SoundMixer::_Load", (const void*)_managed_internal_SoundMixerLoad),
	MethodType("Yonai.SoundMixer::_GetName", (const void*)_managed_internal_SoundMixerGetName),
	MethodType("Yonai.SoundMixer::_SetName", (const void*)_managed_internal_SoundMixerSetName),
	MethodType("Yonai.SoundMixer::_GetVolume", (const void*)_managed_internal_SoundMixerGetVolume),
	MethodType("Yonai.SoundMixer::_SetVolume", (const void*)_managed_internal_SoundMixerSetVolume),
	MethodType("Yonai.SoundMixer::_GetParent", (const void*)_managed_internal_SoundMixerGetParent),
	MethodType("Yonai.SoundMixer::_SetParent", (const void*)_managed_internal_SoundMixerSetParent),
	MethodType("Yonai.Camera::_SetRenderTarget", (const void*)_managed_internal_CameraSetRenderTarget),
	MethodType("Yonai.Camera::_GetFar", (const void*)_managed_internal_CameraGetFar),
	MethodType("Yonai.Camera::_SetFar", (const void*)_managed_internal_CameraSetFar),
	MethodType("Yonai.Camera::_GetNear", (const void*)_managed_internal_CameraGetNear),
	MethodType("Yonai.Camera::_SetNear", (const void*)_managed_internal_CameraSetNear),
	MethodType("Yonai.Camera::_GetRenderTarget", (const void*)_managed_internal_CameraGetRenderTarget),
	MethodType("Yonai.Camera::_GetFieldOfView", (const void*)_managed_internal_CameraGetFieldOfView),
	MethodType("Yonai.Camera::_SetFieldOfView", (const void*)_managed_internal_CameraSetFieldOfView),
	MethodType("Yonai.Camera::_GetOrthographic", (const void*)_managed_internal_CameraGetOrthographic),
	MethodType("Yonai.Camera::_SetOrthographic", (const void*)_managed_internal_CameraSetOrthographic),
	MethodType("Yonai.Camera::_GetOrthographicSize", (const void*)_managed_internal_CameraGetOrthographicSize),
	MethodType("Yonai.Camera::_SetOrthographicSize", (const void*)_managed_internal_CameraSetOrthographicSize),
	MethodType("Yonai.MeshRenderer::_GetMesh", (const void*)_managed_internal_MeshRendererGetMesh),
	MethodType("Yonai.MeshRenderer::_SetMesh", (const void*)_managed_internal_MeshRendererSetMesh),
	MethodType("Yonai.MeshRenderer::_GetMaterial", (const void*)_managed_internal_MeshRendererGetMaterial),
	MethodType("Yonai.MeshRenderer::_SetMaterial", (const void*)_managed_internal_MeshRendererSetMaterial),
	MethodType("Yonai.SoundSource::_GetSound", (const void*)_managed_internal_SoundSourceGetSound),
	MethodType("Yonai.SoundSource::_SetSound", (const void*)_managed_internal_SoundSourceSetSound),
	MethodType("Yonai.SoundSource::_Play", (const void*)_managed_internal_SoundSourcePlay),
	MethodType("Yonai.SoundSource::_Pause", (const void*)_managed_internal_SoundSourcePause),
	MethodType("Yonai.SoundSource::_Stop", (const void*)_managed_internal_SoundSourceStop),
	MethodType("Yonai.SoundSource::_Seek", (const void*)_managed_internal_SoundSourceSeek),
	MethodType("Yonai.SoundSource::_IsPlaying", (const void*)_managed_internal_SoundSourceIsPlaying),
	MethodType("Yonai.SoundSource::_GetState", (const void*)_managed_internal_SoundSourceGetState),
	MethodType("Yonai.SoundSource::_GetPlayTime", (const void*)_managed_internal_SoundSourceGetPlayTime),
	MethodType("Yonai.SoundSource::_GetLength", (const void*)_managed_internal_SoundSourceGetLength),
	MethodType("Yonai.SoundSource::_GetLooping", (const void*)_managed_internal_SoundSourceGetLooping),
	MethodType("Yonai.SoundSource::_SetLooping", (const void*)_managed_internal_SoundSourceSetLooping),
	MethodType("Yonai.SoundSource::_GetSpatialization", (const void*)_managed_internal_SoundSourceGetSpatialization),
	MethodType("Yonai.SoundSource::_SetSpatialization", (const void*)_managed_internal_SoundSourceSetSpatialization),
	MethodType("Yonai.SoundSource::_GetMixer", (const void*)_managed_internal_SoundSourceGetMixer),
	MethodType("Yonai.SoundSource::_SetMixer", (const void*)_managed_internal_SoundSourceSetMixer),
	MethodType("Yonai.SoundSource::_GetPanning", (const void*)_managed_internal_SoundSourceGetPanning),
	MethodType("Yonai.SoundSource::_SetPanning", (const void*)_managed_internal_SoundSourceSetPanning),
	MethodType("Yonai.SoundSource::_GetVolume", (const void*)_managed_internal_SoundSourceGetVolume),
	MethodType("Yonai.SoundSource::_SetVolume", (const void*)_managed_internal_SoundSourceSetVolume),
	MethodType("Yonai.SoundSource::_GetPitch", (const void*)_managed_internal_SoundSourceGetPitch),
	MethodType("Yonai.SoundSource::_SetPitch", (const void*)_managed_internal_SoundSourceSetPitch),
	MethodType("Yonai.Transform::_GetPosition", (const void*)_managed_internal_TransformGetPosition),
	MethodType("Yonai.Transform::_SetPosition", (const void*)_managed_internal_TransformSetPosition),
	MethodType("Yonai.Transform::_GetGlobalPosition", (const void*)_managed_internal_TransformGetGlobalPosition),
	MethodType("Yonai.Transform::_GetRotation", (const void*)_managed_internal_TransformGetRotation),
	MethodType("Yonai.Transform::_SetRotation", (const void*)_managed_internal_TransformSetRotation),
	MethodType("Yonai.Transform::_GetGlobalRotation", (const void*)_managed_internal_TransformGetGlobalRotation),
	MethodType("Yonai.Transform::_GetScale", (const void*)_managed_internal_TransformGetScale),
	MethodType("Yonai.Transform::_SetScale", (const void*)_managed_internal_TransformSetScale),
	MethodType("Yonai.Transform::_GetGlobalScale", (const void*)_managed_internal_TransformGetGlobalScale),
	MethodType("Yonai.Transform::_GetUp", (const void*)_managed_internal_TransformGetUp),
	MethodType("Yonai.Transform::_GetRight", (const void*)_managed_internal_TransformGetRight),
	MethodType("Yonai.Transform::_GetForward", (const void*)_managed_internal_TransformGetForward),
	MethodType("Yonai.Transform::_GetParent", (const void*)_managed_internal_TransformGetParent),
	MethodType("Yonai.Transform::_SetParent", (const void*)_managed_internal_TransformSetParent),
	MethodType("Yonai.Transform::_GetChildren", (const void*)_managed_internal_TransformGetChildren),
	MethodType("Yonai.Transform::_AddChild", (const void*)_managed_internal_TransformAddChild),
	MethodType("Yonai.Transform::_RemoveChild", (const void*)_managed_internal_TransformRemoveChild),
	MethodType("Yonai.Grapghics.Framebuffer::_Load", (const void*)_managed_internal_FramebufferLoad),
	MethodType("Yonai.Graphics.Framebuffer::_Destroy", (const void*)_managed_internal_FramebufferDestroy),
	MethodType("Yonai.Graphics.Framebuffer::_Bind", (const void*)_managed_internal_FramebufferBind),
	MethodType("Yonai.Graphics.Framebuffer::_Unind", (const void*)_managed_internal_FramebufferUnind),
	MethodType("Yonai.Graphics.Framebuffer::_SetSamples", (const void*)_managed_internal_FramebufferSetSamples),
	MethodType("Yonai.Graphics.Framebuffer::_SetResolution", (const void*)_managed_internal_FramebufferSetResolution),
	MethodType("Yonai.Graphics.Framebuffer::_GetAttachmentCount", (const void*)_managed_internal_FramebufferGetAttachmentCount),
	MethodType("Yonai.Graphics.Framebuffer::_GetColourAttachmentCount", (const void*)_managed_internal_FramebufferGetColourAttachmentCount),
	MethodType("Yonai.Graphics.Framebuffer::_CopyAttachmentTo", (const void*)_managed_internal_FramebufferCopyAttachmentTo),
	MethodType("Yonai.Graphics.Framebuffer::_BlitTo", (const void*)_managed_internal_FramebufferBlitTo),
	MethodType("Yonai.Graphics.Framebuffer::_GetAttachments", (const void*)_managed_internal_FramebufferGetAttachments),
	MethodType("Yonai.Graphics.Material::_Load", (const void*)_managed_internal_MaterialLoad),
	MethodType("Yonai.Graphics.Material::_GetAlbedo", (const void*)_managed_internal_MaterialGetAlbedo),
	MethodType("Yonai.Graphics.Material::_SetAlbedo", (const void*)_managed_internal_MaterialSetAlbedo),
	MethodType("Yonai.Graphics.Material::_GetTextureCoordinateScale", (const void*)_managed_internal_MaterialGetTextureCoordinateScale),
	MethodType("Yonai.Graphics.Material::_SetTextureCoordinateScale", (const void*)_managed_internal_MaterialSetTextureCoordinateScale),
	MethodType("Yonai.Graphics.Material::_GetTextureCoordinateOffset", (const void*)_managed_internal_MaterialGetTextureCoordinateOffset),
	MethodType("Yonai.Graphics.Material::_SetTextureCoordinateOffset", (const void*)_managed_internal_MaterialSetTextureCoordinateOffset),
	MethodType("Yonai.Graphics.Material::_GetShader", (const void*)_managed_internal_MaterialGetShader),
	MethodType("Yonai.Graphics.Material::_SetShader", (const void*)_managed_internal_MaterialSetShader),
	MethodType("Yonai.Graphics.Material::_GetAlbedoMap", (const void*)_managed_internal_MaterialGetAlbedoMap),
	MethodType("Yonai.Graphics.Material::_SetAlbedoMap", (const void*)_managed_internal_MaterialSetAlbedoMap),
	MethodType("Yonai.Graphics.Material::_GetAlphaClipping", (const void*)_managed_internal_MaterialGetAlphaClipping),
	MethodType("Yonai.Graphics.Material::_SetAlphaClipping", (const void*)_managed_internal_MaterialSetAlphaClipping),
	MethodType("Yonai.Graphics.Material::_GetAlphaClipThreshold", (const void*)_managed_internal_MaterialGetAlphaClipThreshold),
	MethodType("Yonai.Graphics.Material::_SetAlphaClipThreshold", (const void*)_managed_internal_MaterialSetAlphaClipThreshold),
	MethodType("Yonai.Graphics.Material::_GetRoughness", (const void*)_managed_internal_MaterialGetRoughness),
	MethodType("Yonai.Graphics.Material::_SetRoughness", (const void*)_managed_internal_MaterialSetRoughness),
	MethodType("Yonai.Graphics.Material::_GetRoughnessMap", (const void*)_managed_internal_MaterialGetRoughnessMap),
	MethodType("Yonai.Graphics.Material::_SetRoughnessMap", (const void*)_managed_internal_MaterialSetRoughnessMap),
	MethodType("Yonai.Graphics.Material::_GetMetalness", (const void*)_managed_internal_MaterialGetMetalness),
	MethodType("Yonai.Graphics.Material::_SetMetalness", (const void*)_managed_internal_MaterialSetMetalness),
	MethodType("Yonai.Graphics.Material::_GetMetalnessMap", (const void*)_managed_internal_MaterialGetMetalnessMap),
	MethodType("Yonai.Graphics.Material::_SetMetalnessMap", (const void*)_managed_internal_MaterialSetMetalnessMap),
	MethodType("Yonai.Graphics.Material::_GetNormalMap", (const void*)_managed_internal_MaterialGetNormalMap),
	MethodType("Yonai.Graphics.Material::_SetNormalMap", (const void*)_managed_internal_MaterialSetNormalMap),
	MethodType("Yonai.Graphics.Material::_GetAmbientOcclusionMap", (const void*)_managed_internal_MaterialGetAmbientOcclusionMap),
	MethodType("Yonai.Graphics.Material::_SetAmbientOcclusionMap", (const void*)_managed_internal_MaterialSetAmbientOcclusionMap),
	MethodType("Yonai.Graphics.Material::_GetTransparent", (const void*)_managed_internal_MaterialGetTransparent),
	MethodType("Yonai.Graphics.Material::_SetTransparent", (const void*)_managed_internal_MaterialSetTransparent),
	MethodType("Yonai.Graphics.Mesh::_Load", (const void*)_managed_internal_MeshLoad),
	MethodType("Yonai.Graphics.Mesh::_Import", (const void*)_managed_internal_MeshImport),
	MethodType("Yonai.Graphics.Mesh::_GetVertices", (const void*)_managed_internal_MeshGetVertices),
	MethodType("Yonai.Graphics.Mesh::_SetVertices", (const void*)_managed_internal_MeshSetVertices),
	MethodType("Yonai.Graphics.Mesh::_GetIndices", (const void*)_managed_internal_MeshGetIndices),
	MethodType("Yonai.Graphics.Mesh::_SetIndices", (const void*)_managed_internal_MeshSetIndices),
	MethodType("Yonai.Graphics.Model::_Load", (const void*)_managed_internal_ModelLoad),
	MethodType("Yonai.Graphics.Model::_Import", (const void*)_managed_internal_ModelImport),
	MethodType("Yonai.Graphics.Model::_GetMeshes", (const void*)_managed_internal_ModelGetMeshes),
	MethodType("Yonai.Graphics.Pipelines.NativeRenderPipeline::_Draw", (const void*)_managed_internal_NativeRenderPipelineDraw),
	MethodType("Yonai.Graphics.Pipelines.NativeRenderPipeline::_SetResolution", (const void*)_managed_internal_NativeRenderPipelineSetResolution),
	MethodType("Yonai.Graphics.Pipelines.NativeRenderPipeline::_GetResolution", (const void*)_managed_internal_NativeRenderPipelineGetResolution),
	MethodType("Yonai.Graphics.Pipelines.NativeRenderPipeline::_GetOutput", (const void*)_managed_internal_NativeRenderPipelineGetOutput),
	MethodType("Yonai.Graphics.RenderTexture::_Load", (const void*)_managed_internal_RenderTextureLoad),
	MethodType("Yonai.Graphics.RenderTexture::_Destroy", (const void*)_managed_internal_RenderTextureDestroy),
	MethodType("Yonai.Graphics.RenderTexture::_Bind", (const void*)_managed_internal_RenderTextureBind),
	MethodType("Yonai.Graphics.RenderTexture::_Unbind", (const void*)_managed_internal_RenderTextureUnbind),
	MethodType("Yonai.Graphics.RenderTexture::_SetFormat", (const void*)_managed_internal_RenderTextureSetFormat),
	MethodType("Yonai.Graphics.RenderTexture::_SetSamples", (const void*)_managed_internal_RenderTextureSetSamples),
	MethodType("Yonai.Graphics.RenderTexture::_SetResolution", (const void*)_managed_internal_RenderTextureSetResolution),
	MethodType("Yonai.Graphics.RenderTexture::_GetFormat", (const void*)_managed_internal_RenderTextureGetFormat),
	MethodType("Yonai.Graphics.RenderTexture::_GetSamples", (const void*)_managed_internal_RenderTextureGetSamples),
	MethodType("Yonai.Graphics.RenderTexture::_GetResolution", (const void*)_managed_internal_RenderTextureGetResolution),
	MethodType("Yonai.Graphics.RenderTexture::_CopyTo", (const void*)_managed_internal_RenderTextureCopyTo),
	MethodType("Yonai.Graphics.Shader::_Load", (const void*)_managed_internal_ShaderLoad),
	MethodType("Yonai.Graphics.Shader::_Bind", (const void*)_managed_internal_ShaderBind),
	MethodType("Yonai.Graphics.Shader::_Unbind", (const void*)_managed_internal_ShaderUnbind),
	MethodType("Yonai.Graphics.Shader::_UpdateStages", (const void*)_managed_internal_ShaderUpdateStages),
	MethodType("Yonai.Graphics.Shader::_UnloadStages", (const void*)_managed_internal_ShaderUnloadStages),
	MethodType("Yonai.Graphics.Shader::_GetStages", (const void*)_managed_internal_ShaderGetStages),
	MethodType("Yonai.Graphics.Shader::_Set_int", (const void*)_managed_internal_ShaderSet_int),
	MethodType("Yonai.Graphics.Shader::_SetStr_int", (const void*)_managed_internal_ShaderSetStr_int),
	MethodType("Yonai.Graphics.Shader::_Set_bool", (const void*)_managed_internal_ShaderSet_bool),
	MethodType("Yonai.Graphics.Shader::_SetStr_bool", (const void*)_managed_internal_ShaderSetStr_bool),
	MethodType("Yonai.Graphics.Shader::_Set_float", (const void*)_managed_internal_ShaderSet_float),
	MethodType("Yonai.Graphics.Shader::_SetStr_float", (const void*)_managed_internal_ShaderSetStr_float),
	MethodType("Yonai.Graphics.Shader::_Set_double", (const void*)_managed_internal_ShaderSet_double),
	MethodType("Yonai.Graphics.Shader::_SetStr_double", (const void*)_managed_internal_ShaderSetStr_double),
	MethodType("Yonai.Graphics.Shader::_Set_vec2", (const void*)_managed_internal_ShaderSet_vec2),
	MethodType("Yonai.Graphics.Shader::_SetStr_vec2", (const void*)_managed_internal_ShaderSetStr_vec2),
	MethodType("Yonai.Graphics.Shader::_Set_vec3", (const void*)_managed_internal_ShaderSet_vec3),
	MethodType("Yonai.Graphics.Shader::_SetStr_vec3", (const void*)_managed_internal_ShaderSetStr_vec3),
	MethodType("Yonai.Graphics.Shader::_Set_vec4", (const void*)_managed_internal_ShaderSet_vec4),
	MethodType("Yonai.Graphics.Shader::_SetStr_vec4", (const void*)_managed_internal_ShaderSetStr_vec4),
	MethodType("Yonai.Graphics.Shader::_Set_mat3", (const void*)_managed_internal_ShaderSet_mat3),
	MethodType("Yonai.Graphics.Shader::_SetStr_mat3", (const void*)_managed_internal_ShaderSetStr_mat3),
	MethodType("Yonai.Graphics.Shader::_Set_mat4", (const void*)_managed_internal_ShaderSet_mat4),
	MethodType("Yonai.Graphics.Shader::_SetStr_mat4", (const void*)_managed_internal_ShaderSetStr_mat4),
	MethodType("Yonai.Graphics.Texture::_Load", (const void*)_managed_internal_TextureLoad),
	MethodType("Yonai.Graphics.Texture::_Upload", (const void*)_managed_internal_TextureUpload),
	MethodType("Yonai.Graphics.Texture::_Bind", (const void*)_managed_internal_TextureBind),
	MethodType("Yonai.Graphics.Texture::_GetResolution", (const void*)_managed_internal_TextureGetResolution),
	MethodType("Yonai.Graphics.Texture::_GetHDR", (const void*)_managed_internal_TextureGetHDR),
	MethodType("Yonai.Graphics.Texture::_GetFilter", (const void*)_managed_internal_TextureGetFilter),
	MethodType("Yonai.Graphics.Texture::_IsValid", (const void*)_managed_internal_TextureIsValid),
	MethodType("Yonai.IO.Clipboard::_SetText", (const void*)_managed_internal_ClipboardSetText),
	MethodType("Yonai.IO.Clipboard::_GetText", (const void*)_managed_internal_ClipboardGetText),
	MethodType("Yonai.Audio::_GetDeviceCount", (const void*)_managed_internal_AudioGetDeviceCount),
	MethodType("Yonai.Audio::_GetDefaultDevice", (const void*)_managed_internal_AudioGetDefaultDevice),
	MethodType("Yonai.Audio::_GetDeviceName", (const void*)_managed_internal_AudioGetDeviceName),
	MethodType("Yonai.Audio::_SetOutputDevice", (const void*)_managed_internal_AudioSetOutputDevice),
	MethodType("Yonai.Audio::_GetOutputDevice", (const void*)_managed_internal_AudioGetOutputDevice),
	MethodType("Yonai.BaseGameLauncher::_LoadAssembly", (const void*)_managed_internal_BaseGameLauncherLoadAssembly),
	MethodType("Yonai.Input::_InputEnabled", (const void*)_managed_internal_InputInputEnabled),
	MethodType("Yonai.Input::_InputEnable", (const void*)_managed_internal_InputInputEnable),
	MethodType("Yonai.Input::_IsKeyUp", (const void*)_managed_internal_InputIsKeyUp),
	MethodType("Yonai.Input::_IsKeyDown", (const void*)_managed_internal_InputIsKeyDown),
	MethodType("Yonai.Input::_IsKeyPressed", (const void*)_managed_internal_InputIsKeyPressed),
	MethodType("Yonai.Input::_IsKeyReleased", (const void*)_managed_internal_InputIsKeyReleased),
	MethodType("Yonai.Input::_ScrollDelta", (const void*)_managed_internal_InputScrollDelta),
	MethodType("Yonai.Input::_SetMousePosition", (const void*)_managed_internal_InputSetMousePosition),
	MethodType("Yonai.Input::_GetMouseDelta", (const void*)_managed_internal_InputGetMouseDelta),
	MethodType("Yonai.Input::_GetMousePosition", (const void*)_managed_internal_InputGetMousePosition),
	MethodType("Yonai.Input::_IsMouseUp", (const void*)_managed_internal_InputIsMouseUp),
	MethodType("Yonai.Input::_IsMouseDown", (const void*)_managed_internal_InputIsMouseDown),
	MethodType("Yonai.Input::_IsMousePressed", (const void*)_managed_internal_InputIsMousePressed),
	MethodType("Yonai.Input::_IsMouseReleased", (const void*)_managed_internal_InputIsMouseReleased),
	MethodType("Yonai.Input::_IsMouseShowing", (const void*)_managed_internal_InputIsMouseShowing),
	MethodType("Yonai.Input::_GetMouseState", (const void*)_managed_internal_InputGetMouseState),
	MethodType("Yonai.Input::_SetMouseState", (const void*)_managed_internal_InputSetMouseState),
	MethodType("Yonai.Input::_IsGamepadConnected", (const void*)_managed_internal_InputIsGamepadConnected),
	MethodType("Yonai.Input::_GetAxis", (const void*)_managed_internal_InputGetAxis),
	MethodType("Yonai.Input::_IsButtonUp", (const void*)_managed_internal_InputIsButtonUp),
	MethodType("Yonai.Input::_IsButtonDown", (const void*)_managed_internal_InputIsButtonDown),
	MethodType("Yonai.Log::_NativeLog", (const void*)_managed_internal_LogNativeLog),
	MethodType("Yonai.Quaternion::_ToEuler", (const void*)_managed_internal_QuaternionToEuler),
	MethodType("Yonai.Quaternion::_FromEuler", (const void*)_managed_internal_QuaternionFromEuler),
	MethodType("Yonai.Quaternion::_Multiply", (const void*)_managed_internal_QuaternionMultiply),
	MethodType("Yonai.Quaternion::_MultiplyVector3", (const void*)_managed_internal_QuaternionMultiplyVector3),
	MethodType("Yonai.Quaternion::_Inverse", (const void*)_managed_internal_QuaternionInverse),
	MethodType("Yonai.Quaternion::_InverseInline", (const void*)_managed_internal_QuaternionInverseInline),
	MethodType("Yonai.Quaternion::_AngleAxis", (const void*)_managed_internal_QuaternionAngleAxis),
	MethodType("Yonai.Quaternion::_Normalise", (const void*)_managed_internal_QuaternionNormalise),
	MethodType("Yonai.Random::_Seed", (const void*)_managed_internal_RandomSeed),
	MethodType("Yonai.Random::_Int", (const void*)_managed_internal_RandomInt),
	MethodType("Yonai.Random::_Float", (const void*)_managed_internal_RandomFloat),
	MethodType("Yonai.Resource::_Duplicate", (const void*)_managed_internal_ResourceDuplicate),
	MethodType("Yonai.Resource::_GetPath", (const void*)_managed_internal_ResourceGetPath),
	MethodType("Yonai.Resource::_GetID", (const void*)_managed_internal_ResourceGetID),
	MethodType("Yonai.Resource::_CreateID", (const void*)_managed_internal_ResourceCreateID),
	MethodType("Yonai.Resource::_Unload", (const void*)_managed_internal_ResourceUnload),
	MethodType("Yonai.Resource::_GetInstance", (const void*)_managed_internal_ResourceGetInstance),
	MethodType("Yonai.Resource::_Exists", (const void*)_managed_internal_ResourceExists),
	MethodType("Yonai.Resource::_Print", (const void*)_managed_internal_ResourcePrint),
	MethodType("Yonai.SpriteRenderer::_GetColour", (const void*)_managed_internal_SpriteRendererGetColour),
	MethodType("Yonai.SpriteRenderer::_SetColour", (const void*)_managed_internal_SpriteRendererSetColour),
	MethodType("Yonai.SpriteRenderer::_GetSprite", (const void*)_managed_internal_SpriteRendererGetSprite),
	MethodType("Yonai.SpriteRenderer::_SetSprite", (const void*)_managed_internal_SpriteRendererSetSprite),
	MethodType("Yonai.SpriteRenderer::_GetShader", (const void*)_managed_internal_SpriteRendererGetShader),
	MethodType("Yonai.SpriteRenderer::_SetShader", (const void*)_managed_internal_SpriteRendererSetShader),
	MethodType("Yonai.Time::_FPS", (const void*)_managed_internal_TimeFPS),
	MethodType("Yonai.Time::_DeltaTime", (const void*)_managed_internal_TimeDeltaTime),
	MethodType("Yonai.Time::_TimeSinceLaunch", (const void*)_managed_internal_TimeTimeSinceLaunch),
	MethodType("Yonai.Vector2::_Magnitude", (const void*)_managed_internal_Vector2Magnitude),
	MethodType("Yonai.Vector2::_Distance", (const void*)_managed_internal_Vector2Distance),
	MethodType("Yonai.Vector3::_Distance", (const void*)_managed_internal_Vector3Distance),
	MethodType("Yonai.Vector3::_Dot", (const void*)_managed_internal_Vector3Dot),
	MethodType("Yonai.Vector3::_Magnitude", (const void*)_managed_internal_Vector3Magnitude),
	MethodType("Yonai.Vector4::_Magnitude", (const void*)_managed_internal_Vector4Magnitude),
	MethodType("Yonai.Window::_SetResolution", (const void*)_managed_internal_WindowSetResolution),
	MethodType("Yonai.Window::_GetResolution", (const void*)_managed_internal_WindowGetResolution),
	MethodType("Yonai.Window::_SetFullscreenMode", (const void*)_managed_internal_WindowSetFullscreenMode),
	MethodType("Yonai.Window::_GetFullscreenMode", (const void*)_managed_internal_WindowGetFullscreenMode),
	MethodType("Yonai.Window::_GetContentScaling", (const void*)_managed_internal_WindowGetContentScaling),
	MethodType("Yonai.Screen::_GetVideoModes", (const void*)_managed_internal_ScreenGetVideoModes),
	MethodType("Yonai.Screen::_GetVideoMode", (const void*)_managed_internal_ScreenGetVideoMode),
	MethodType("Yonai.Screen::_SetVideoMode", (const void*)_managed_internal_ScreenSetVideoMode),
	MethodType("Yonai.Window::_CenterOnDisplay", (const void*)_managed_internal_WindowCenterOnDisplay),
	MethodType("Yonai.Window::_GetTitle", (const void*)_managed_internal_WindowGetTitle),
	MethodType("Yonai.Window::_SetTitle", (const void*)_managed_internal_WindowSetTitle),
	MethodType("Yonai.Window::_RequestedToClose", (const void*)_managed_internal_WindowRequestedToClose),
	MethodType("Yonai.Window::_GetVSync", (const void*)_managed_internal_WindowGetVSync),
	MethodType("Yonai.Window::_SetVSync", (const void*)_managed_internal_WindowSetVSync),
	MethodType("Yonai.World::_Get", (const void*)_managed_internal_WorldGet),
	MethodType("Yonai.World::_GetAll", (const void*)_managed_internal_WorldGetAll),
	MethodType("Yonai.World::_Exists", (const void*)_managed_internal_WorldExists),
	MethodType("Yonai.World::_Destroy", (const void*)_managed_internal_WorldDestroy),
	MethodType("Yonai.World::_Create", (const void*)_managed_internal_WorldCreate),
	MethodType("Yonai.World::_SetName", (const void*)_managed_internal_WorldSetName),
	MethodType("Yonai.World::_CreateEntity", (const void*)_managed_internal_WorldCreateEntity),
	MethodType("Yonai.World::_CreateEntityID", (const void*)_managed_internal_WorldCreateEntityID),
	MethodType("Yonai.World::_DestroyEntity", (const void*)_managed_internal_WorldDestroyEntity),
	MethodType("Yonai.World::_HasEntity", (const void*)_managed_internal_WorldHasEntity),
	MethodType("Yonai.World::_GetEntities", (const void*)_managed_internal_WorldGetEntities),
	MethodType("Yonai.World::_GetComponents", (const void*)_managed_internal_WorldGetComponents),
	MethodType("Yonai.World::_GetComponentsMultiple", (const void*)_managed_internal_WorldGetComponentsMultiple),
	MethodType("Yonai.Entity::_HasComponent", (const void*)_managed_internal_EntityHasComponent),
	MethodType("Yonai.Entity::_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	MethodType("Yonai.Entity::_AddComponent", (const void*)_managed_internal_EntityAddComponent),
	MethodType("Yonai.Entity::_RemoveComponent", (const void*)_managed_internal_EntityRemoveComponent),
	MethodType("Yonai.Entity::_GetComponent", (const void*)_managed_internal_EntityGetComponent),
	MethodType("Yonai.Entity::_GetComponents", (const void*)_managed_internal_EntityGetComponents),
	MethodType("Yonai.World::_HasSystem", (const void*)_managed_internal_WorldHasSystem),
	MethodType("Yonai.World::_GetSystem", (const void*)_managed_internal_WorldGetSystem),
	MethodType("Yonai.World::_GetSystems", (const void*)_managed_internal_WorldGetSystems),
	MethodType("Yonai.World::_AddSystem", (const void*)_managed_internal_WorldAddSystem),
	MethodType("Yonai.World::_RemoveSystem", (const void*)_managed_internal_WorldRemoveSystem),
	MethodType("Yonai.World::_EnableSystem", (const void*)_managed_internal_WorldEnableSystem),
	MethodType("Yonai.Graphics.Renderer::_GetPipeline", (const void*)_managed_internal_RendererGetPipeline),
	MethodType("Yonai.Graphics.Renderer::_Draw", (const void*)_managed_internal_RendererDraw),
	MethodType("Yonai.Graphics.Renderer::_DrawFromCamera", (const void*)_managed_internal_RendererDrawFromCamera),
	MethodType("Yonai.SceneManager::_Load", (const void*)_managed_internal_SceneManagerLoad),
	MethodType("Yonai.SceneManager::_LoadAdditive", (const void*)_managed_internal_SceneManagerLoadAdditive),
	MethodType("Yonai.SceneManager::_Unload", (const void*)_managed_internal_SceneManagerUnload),
	MethodType("Yonai.SceneManager::_UnloadAll", (const void*)_managed_internal_SceneManagerUnloadAll),
	MethodType("Yonai.SceneManager::_GetActiveScenes", (const void*)_managed_internal_SceneManagerGetActiveScenes),
	MethodType("Yonai.NativeSystem::_GetHandle", (const void*)_managed_internal_NativeSystemGetHandle)
};