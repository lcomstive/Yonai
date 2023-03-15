#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

ADD_MANAGED_METHOD(Transform, GetPosition, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->Position; }
ADD_MANAGED_METHOD(Transform, SetPosition, void, (void* handle, glm::vec3* input)) { ((Transform*)handle)->Position = *input; }

ADD_MANAGED_METHOD(Transform, GetRotation, void, (void* handle, glm::quat* output)) { *output = ((Transform*)handle)->Rotation; }
ADD_MANAGED_METHOD(Transform, SetRotation, void, (void* handle, glm::quat* input)) { ((Transform*)handle)->Rotation = *input; }

ADD_MANAGED_METHOD(Transform, GetScale, void, (void* handle, glm::vec3* output)) { *output = ((Transform*)handle)->Scale; }
ADD_MANAGED_METHOD(Transform, SetScale, void, (void* handle, glm::vec3* input)) { ((Transform*)handle)->Scale = *input; }

ADD_MANAGED_METHOD(Transform, GetUp, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Up(); }
ADD_MANAGED_METHOD(Transform, GetRight, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Right(); }
ADD_MANAGED_METHOD(Transform, GetForward, void, (void* handle, glm::vec3* value)) { *value = ((Transform*)handle)->Forward(); }