#include <spdlog/spdlog.h>
#include <Yonai/Resource.hpp>
#include <Yonai/Resource.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Graphics/Texture.hpp>
#include <Yonai/Graphics/Material.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Graphics;

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

#pragma region Internal Calls
// _Load(string path, out uint resourceID, out IntPtr handle);
ADD_MANAGED_METHOD(Material, Load, void, (MonoString* pathRaw, uint64_t* resourceID, void** outHandle), Yonai.Graphics)
{
	if (*resourceID == InvalidResourceID)
		*resourceID = ResourceID(); // Assign resource ID
	
	char* path = mono_string_to_utf8(pathRaw);
	Resource::Load<Material>(*resourceID, path);
	*outHandle = Resource::Get<Material>(*resourceID);
	mono_free(path);
}

ADD_MANAGED_GET_SET(Material, Shader, uint64_t, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, AlbedoMap, uint64_t, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, AlphaClipping, bool, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, AlphaClipThreshold, float, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, Roughness, float, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, RoughnessMap, uint64_t, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, Metalness, float, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, MetalnessMap, uint64_t, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, NormalMap, uint64_t, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, AmbientOcclusionMap, uint64_t, Yonai.Graphics)
ADD_MANAGED_GET_SET(Material, Transparent, bool, Yonai.Graphics)

ADD_MANAGED_METHOD(Material, GetAlbedo, void, (void* handle, glm::vec4* value), Yonai.Graphics)
{ *value = ((Material*)handle)->Albedo; }
ADD_MANAGED_METHOD(Material, SetAlbedo, void, (void* handle, glm::vec4* value), Yonai.Graphics)
{ ((Material*)handle)->Albedo = *value; }

ADD_MANAGED_METHOD(Material, GetTextureCoordinateScale, void, (void* handle, glm::vec2* value), Yonai.Graphics)
{ *value = ((Material*)handle)->TextureCoordinateScale; }
ADD_MANAGED_METHOD(Material, SetTextureCoordinateScale, void, (void* handle, glm::vec2* value), Yonai.Graphics)
{ ((Material*)handle)->TextureCoordinateScale = *value; }

ADD_MANAGED_METHOD(Material, GetTextureCoordinateOffset, void, (void* handle, glm::vec2* value), Yonai.Graphics)
{ *value = ((Material*)handle)->TextureCoordinateOffset; }
ADD_MANAGED_METHOD(Material, SetTextureCoordinateOffset, void, (void* handle, glm::vec2* value), Yonai.Graphics)
{ ((Material*)handle)->TextureCoordinateOffset = *value; }
#pragma endregion