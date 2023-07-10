#pragma once
#include <glm/glm.hpp>
#include <Yonai/ResourceID.hpp>
#include <Yonai/Components/Component.hpp>

namespace Yonai::Components
{
	struct SpriteRenderer : public Component
	{
		/// <summary>
		/// Texture to draw
		/// </summary>
		ResourceID Sprite = InvalidResourceID;

		/// <summary>
		/// Tint to give drawn sprite
		/// </summary>
		glm::vec4 Colour = { 1, 1, 1, 1 };

		/// <summary>
		/// Shader program to draw sprite
		/// </summary>
		ResourceID Shader = InvalidResourceID;
	};
}