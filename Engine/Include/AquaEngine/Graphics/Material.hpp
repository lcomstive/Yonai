#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Graphics/Shader.hpp>

namespace AquaEngine::Graphics
{
	struct Material
	{
		/// <summary>
		/// Shader program to draw attached meshes
		/// </summary>
		ResourceID Shader = InvalidResourceID;

		/// <summary>
		/// This color is multiplied with other colors, such as the albedoMap or vertex colors.
		/// If transparency is enabled (AlphaClipping), the alpha channel is used to adjust the opacity,
		/// with 1 meaning fully opaque and 0 meaning fully transparent.
		/// 
		/// Default is white.
		/// </summary>
		glm::vec4 Albedo = { 1.0f, 1.0f, 1.0f, 1.0f };

		/// <summary>
		/// Per-pixel albedo values
		/// </summary>
		ResourceID AlbedoMap = InvalidResourceID;

		/// <summary>
		/// When enabled, albedo alpha's under AlphaClipThreshold are discarded.
		/// 
		/// Default is disabled.
		/// </summary>
		bool AlphaClipping = false;

		/// <summary>
		/// When AlphaClipping is enabled, alpha values below this threshold are discarded.
		/// 
		/// Default is 0.1.
		/// </summary>
		float AlphaClipThreshold = 0.1f;

		/// <summary>
		/// Scales UV texture coordinates.
		/// 
		/// Default is (1.0, 1.0).
		/// </summary>
		glm::vec2 TextureCoordinateScale = { 1.0f, 1.0f };

		/// <summary>
		/// Shifts UV texture coordinates.
		/// 
		/// Default is (0.0, 0.0).
		/// </summary>
		glm::vec2 TextureCoordinateOffset = { 0.0f, 0.0f };

		/// <summary>
		/// Defines how smooth a surface is.
		/// 
		/// Range is 0.0-1.0.
		/// 
		/// Default is 0.
		/// </summary>
		float Roughness = 0.0f;

		/// <summary>
		/// Defines how smooth a surface is on a per-pixel basis.
		/// </summary>
		ResourceID RoughnessMap = InvalidResourceID;

		/// <summary>
		/// Defines how reflective a surface is.
		/// 
		/// Range is 0.0-1.0.
		/// 
		/// Default is 0.
		/// </summary>
		float Metalness = 0.0f;

		/// <summary>
		/// Defines how reflective a surface is on a per-pixel basis.
		/// </summary>
		ResourceID MetalnessMap = InvalidResourceID;

		/// <summary>
		/// Simulates extra details on a mesh
		/// </summary>
		ResourceID NormalMap = InvalidResourceID;

		/// <summary>
		/// Simulates corner shadows on a mesh
		/// </summary>
		ResourceID AmbientOcclusionMap = InvalidResourceID;

		/// <summary>
		/// Enables translucency and transparency on rendered mesh
		/// </summary>
		bool Transparent = false;
		
		/// <summary>
		/// Retrieves the relevant shader, binds it and and textures.
		/// Returns the shader, or nullptr if invalid
		/// </summary>
		Graphics::Shader* PrepareShader();
	};
}
