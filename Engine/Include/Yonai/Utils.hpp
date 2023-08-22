#pragma once
#include <string>
#include <glm/glm.hpp>
#include <Yonai/API.hpp>

namespace Yonai
{
	YonaiAPI std::string& ToLower(std::string& input);

	/// <param name="outEulerRotation">Euler rotation, in degrees</param>
	YonaiAPI bool Decompose(glm::mat4& matrix, glm::vec3& outTranslation, glm::vec3& outEulerRotation, glm::vec3& outScale);
}