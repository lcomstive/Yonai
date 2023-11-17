#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <glm/matrix.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace glm;

#pragma region Matrix4
ADD_MANAGED_METHOD(Matrix4, Translate, void, (glm::mat4* input, glm::vec3* translation, glm::mat4* output))
{ *output = translate(*input, *translation); }

ADD_MANAGED_METHOD(Matrix4, Scale, void, (glm::mat4* input, glm::vec3* scale, glm::mat4* output))
{ *output = glm::scale(*input, *scale); }

ADD_MANAGED_METHOD(Matrix4, Rotate, void, (glm::mat4* input, float angle, glm::vec3* axis, glm::mat4* output))
{ *output = rotate(*input, angle, *axis); }

ADD_MANAGED_METHOD(Matrix4, ToMatrix, void, (glm::quat* input, glm::mat4* output))
{ *output = toMat4(*input); }

ADD_MANAGED_METHOD(Matrix4, Invert, void, (glm::mat4* input, glm::mat4* output))
{ *output = inverse(*input); }

ADD_MANAGED_METHOD(Matrix4, LookAt, void, (glm::vec3* eye, glm::vec3* dir, glm::vec3* up, glm::mat4* output))
{ *output = lookAt(*eye, *dir, *up); }

ADD_MANAGED_METHOD(Matrix4, Perspective, void, (float fov, float aspect, float near, float far, glm::mat4* output))
{ *output = perspective(radians(fov), aspect, near, far); }

#pragma endregion