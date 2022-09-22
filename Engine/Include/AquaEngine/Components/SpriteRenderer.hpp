#pragma once
#include <glm/glm.hpp>
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine::Components
{
	struct SpriteRenderer : public Component
	{
		/// <summary>
		/// Texture to draw
		/// </summary>
		ResourceID Sprite;

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