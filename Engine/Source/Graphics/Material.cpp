#include <spdlog/spdlog.h>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Graphics/Material.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;

void BindTexture(unsigned int index, string shaderName, ResourceID textureID, Shader* shader)
{
	shader->Set(shaderName, (int)index);
	glActiveTexture(GL_TEXTURE0 + index);
	
	Texture* texture = Resource::Get<Texture>(textureID);
	if (texture)
		texture->Bind(index);
	else
		glBindTexture(GL_TEXTURE_2D, 0);
}

Shader* Material::PrepareShader()
{
	if (Shader == InvalidResourceID)
		return nullptr; // Invalid shader

	Graphics::Shader* shader = Resource::Get<Graphics::Shader>(Shader);
	shader->Bind();

	shader->Set("albedoColour", Albedo);
	shader->Set("alphaClipping", AlphaClipping);
	shader->Set("alphaClipThreshold", AlphaClipThreshold);
	shader->Set("textureCoordScale", TextureCoordinateScale);
	shader->Set("textureCoordOffset", TextureCoordinateOffset);
	shader->Set("roughness", Roughness);
	shader->Set("metalness", Metalness);
	shader->Set("transparent", Transparent);
	
	shader->Set("hasAlbedoMap", AlbedoMap != InvalidResourceID);
	shader->Set("hasNormalMap", NormalMap != InvalidResourceID);
	shader->Set("hasRoughnessMap", RoughnessMap != InvalidResourceID);
	shader->Set("hasMetalnessMap", MetalnessMap != InvalidResourceID);
	shader->Set("hasAmbientOcclusionMap", AmbientOcclusionMap != InvalidResourceID);

	BindTexture(0, "albedoMap", AlbedoMap, shader);
	BindTexture(1, "normalMap", NormalMap, shader);
	BindTexture(2, "roughnessMap", RoughnessMap, shader);
	BindTexture(3, "metalnessMap", MetalnessMap, shader);
	BindTexture(4, "ambientOcclusionMap", AmbientOcclusionMap, shader);

	return shader;
}
