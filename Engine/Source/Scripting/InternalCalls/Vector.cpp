#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/Scripting/Assembly.hpp>

#pragma region Vector2
float Vector2Magnitude(glm::vec2* input) { return glm::length(*input); }
float Vector2Distance(glm::vec2* a, glm::vec2* b) { return glm::distance(*a, *b); }
#pragma endregion

#pragma region Vector3
float Vector3Distance(glm::vec3* a, glm::vec3* b) { return glm::distance(*a, *b); }
float Vector3Dot(glm::vec3* a, glm::vec3* b) { return glm::dot(*a, *b); }
float Vector3Magnitude(glm::vec3* input) { return glm::length(*input); }
#pragma endregion

#pragma region Vector4
float Vector4Magnitude(glm::vec4* input) { return glm::length(*input); }
#pragma endregion

void AquaEngine::Scripting::Assembly::AddVectorInternalCalls()
{
#pragma region Vector2
	mono_add_internal_call("AquaEngine.Vector2::_aqua_internal_Magnitude", (const void*)Vector2Magnitude);
	mono_add_internal_call("AquaEngine.Vector2::_aqua_internal_Distance", (const void*)Vector2Distance);
#pragma endregion
#pragma region Vector3
	mono_add_internal_call("AquaEngine.Vector3::_aqua_internal_Dot", (const void*)Vector3Dot);
	mono_add_internal_call("AquaEngine.Vector3::_aqua_internal_Distance", (const void*)Vector3Distance);
	mono_add_internal_call("AquaEngine.Vector3::_aqua_internal_Magnitude", (const void*)Vector3Magnitude);
#pragma endregion
#pragma region Vector4
	mono_add_internal_call("AquaEngine.Vector4::_aqua_internal_Magnitude", (const void*)Vector4Magnitude);
#pragma endregion
}