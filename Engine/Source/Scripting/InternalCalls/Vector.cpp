#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

#pragma region Vector2
ADD_MANAGED_METHOD(Vector2, Magnitude, float, (glm::vec2* input))
{ return glm::length(*input); }

ADD_MANAGED_METHOD(Vector2, Distance, float, (glm::vec2* a, glm::vec2* b))
{ return glm::distance(*a, *b); }
#pragma endregion

#pragma region Vector3
ADD_MANAGED_METHOD(Vector3, Distance, float, (glm::vec3* a, glm::vec3* b))
{ return glm::distance(*a, *b); }

ADD_MANAGED_METHOD(Vector3, Dot, float, (glm::vec3* a, glm::vec3* b))
{ return glm::dot(*a, *b); }

ADD_MANAGED_METHOD(Vector3, Magnitude, float, (glm::vec3* input))
{ return glm::length(*input); }
#pragma endregion

#pragma region Vector4
ADD_MANAGED_METHOD(Vector4, Magnitude, float, (glm::vec4* input))
{ return glm::length(*input); }
#pragma endregion
