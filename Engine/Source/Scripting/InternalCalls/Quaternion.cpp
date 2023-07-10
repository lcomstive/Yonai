#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace glm;

ADD_MANAGED_METHOD(Quaternion, ToEuler, void, (glm::quat* input, glm::vec3* output))
{ *output = degrees(eulerAngles(*input)); }

ADD_MANAGED_METHOD(Quaternion, FromEuler, void, (glm::vec3* input, glm::quat* output))
// { *output = toQuat(orientate3(radians(vec3(input->y, input->x, input->z)))); }
{ *output = quat(radians(*input)); }

ADD_MANAGED_METHOD(Quaternion, Multiply, void, (glm::quat* a, glm::quat* b, glm::quat* output))
{ *output = (*a) * (*b); }

ADD_MANAGED_METHOD(Quaternion, MultiplyVector3, void, (glm::quat* a, glm::vec3* b, glm::vec3* output))
{ *output = (*a) * (*b); }

ADD_MANAGED_METHOD(Quaternion, Inverse, void, (glm::quat* input, glm::quat* output))
{ *output = inverse(*input); }

ADD_MANAGED_METHOD(Quaternion, InverseInline, void, (glm::quat* input))
{ *input = inverse(*input); }

ADD_MANAGED_METHOD(Quaternion, AngleAxis, void, (float angle, glm::vec3* axis, glm::quat* output))
{ *output = glm::angleAxis(angle, *axis); }

ADD_MANAGED_METHOD(Quaternion, Normalise, void, (glm::quat* value))
{ glm::normalize(*value); }
