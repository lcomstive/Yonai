#pragma once
#include <glm/glm.hpp>
#include <Yonai/Components/Component.hpp>

namespace Yonai::Components
{
	struct Light : public Component
	{
		/// <summary>
		/// RGB ranging 0.0-1.0
		/// </summary>
		glm::vec3 Colour = { 1.0f, 1.0f, 1.0f };

		float Radius = 10.0f;
	};
}