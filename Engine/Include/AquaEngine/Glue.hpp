#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

using namespace std; 

void _managed_internal_ApplicationExit();
MonoString* _managed_internal_ApplicationGetArg(MonoString* name, MonoString* defaultValue);
bool _managed_internal_ApplicationHasArg(MonoString* name);
void _managed_internal_LogNativeLog(MonoString* rawMsg, int level);
float _managed_internal_TimeFPS();
float _managed_internal_TimeDeltaTime();
float _managed_internal_TimeTimeSinceLaunch();
float _managed_internal_Vector2Magnitude(glm::vec2* input);
float _managed_internal_Vector2Distance(glm::vec2* a, glm::vec2* b);
float _managed_internal_Vector3Distance(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Dot(glm::vec3* a, glm::vec3* b);
float _managed_internal_Vector3Magnitude(glm::vec3* input);
float _managed_internal_Vector4Magnitude(glm::vec4* input);
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


const vector<pair<string, const void*>> _InternalMethods = {
	{ "AquaEngine.Application::_aqua_internal_Exit", _managed_internal_ApplicationExit },
	{ "AquaEngine.Application::_aqua_internal_GetArg", _managed_internal_ApplicationGetArg },
	{ "AquaEngine.Application::_aqua_internal_HasArg", _managed_internal_ApplicationHasArg },
	{ "AquaEngine.Log::_aqua_internal_NativeLog", _managed_internal_LogNativeLog },
	{ "AquaEngine.Time::_aqua_internal_FPS", _managed_internal_TimeFPS },
	{ "AquaEngine.Time::_aqua_internal_DeltaTime", _managed_internal_TimeDeltaTime },
	{ "AquaEngine.Time::_aqua_internal_TimeSinceLaunch", _managed_internal_TimeTimeSinceLaunch },
	{ "AquaEngine.Vector2::_aqua_internal_Magnitude", _managed_internal_Vector2Magnitude },
	{ "AquaEngine.Vector2::_aqua_internal_Distance", _managed_internal_Vector2Distance },
	{ "AquaEngine.Vector3::_aqua_internal_Distance", _managed_internal_Vector3Distance },
	{ "AquaEngine.Vector3::_aqua_internal_Dot", _managed_internal_Vector3Dot },
	{ "AquaEngine.Vector3::_aqua_internal_Magnitude", _managed_internal_Vector3Magnitude },
	{ "AquaEngine.Vector4::_aqua_internal_Magnitude", _managed_internal_Vector4Magnitude },
	{ "AquaEngine.World::_aqua_internal_GetComponentType", _managed_internal_WorldGetComponentType },
	{ "AquaEngine.World::_aqua_internal_Get", _managed_internal_WorldGet },
	{ "AquaEngine.World::_aqua_internal_Exists", _managed_internal_WorldExists },
	{ "AquaEngine.World::_aqua_internal_Destroy", _managed_internal_WorldDestroy },
	{ "AquaEngine.World::_aqua_internal_Create", _managed_internal_WorldCreate },
	{ "AquaEngine.World::_aqua_internal_CreateEntity", _managed_internal_WorldCreateEntity },
	{ "AquaEngine.World::_aqua_internal_DestroyEntity", _managed_internal_WorldDestroyEntity },
	{ "AquaEngine.World::_aqua_internal_HasEntity", _managed_internal_WorldHasEntity },
	{ "AquaEngine.World::_aqua_internal_GetEntities", _managed_internal_WorldGetEntities },
	{ "AquaEngine.World::_aqua_internal_GetComponents", _managed_internal_WorldGetComponents },
	{ "AquaEngine.World::_aqua_internal_GetComponentsMultiple", _managed_internal_WorldGetComponentsMultiple },
	{ "AquaEngine.Entity::_aqua_internal_HasComponent", _managed_internal_EntityHasComponent },
	{ "AquaEngine.Entity::_aqua_internal_GetComponent", _managed_internal_EntityGetComponent },
	{ "AquaEngine.Entity::_aqua_internal_AddComponent", _managed_internal_EntityAddComponent },
	{ "AquaEngine.Entity::_aqua_internal_RemoveComponent", _managed_internal_EntityRemoveComponent },
	{ "AquaEngine.Entity::_aqua_internal_GetComponent", _managed_internal_EntityGetComponent },
	{ "AquaEngine.World::_aqua_internal_HasSystem", _managed_internal_WorldHasSystem },
	{ "AquaEngine.World::_aqua_internal_GetSystem", _managed_internal_WorldGetSystem },
	{ "AquaEngine.World::_aqua_internal_AddSystem", _managed_internal_WorldAddSystem },
	{ "AquaEngine.World::_aqua_internal_RemoveSystem", _managed_internal_WorldRemoveSystem }
};