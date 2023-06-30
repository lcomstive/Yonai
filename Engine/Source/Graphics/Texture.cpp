#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Graphics/Texture.hpp>

#ifndef NDEBUG
#include <AquaEngine/Timer.hpp>
#endif

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;

Texture::Texture() : m_Filter(GL_LINEAR), m_ID(GL_INVALID_VALUE), m_Resolution(), m_HDR(false) { }

Texture::~Texture()
{
	if(m_ID != GL_INVALID_VALUE)
		glDeleteTextures(1, &m_ID);
}

bool Texture::Upload(vector<unsigned char>& textureData, bool hdr, int filter)
{
	if (textureData.empty())
	{
		spdlog::warn("Cannot generate texture with empty path");
		return false;
	}

#ifndef NDEBUG
	Timer profileTimer;
#endif

	// Read image data
	int width, height, channelCount;
	void* data = nullptr;

	// Flip loaded textures, so OpenGL loads them right way up
	stbi_set_flip_vertically_on_load(true);
	
	if (!m_HDR)
		data = stbi_load_from_memory(textureData.data(), (int)textureData.size(), &width, &height, &channelCount, 0);
	else
		data = stbi_loadf_from_memory(textureData.data(), (int)textureData.size(), &width, &height, &channelCount, 0);

	// Check for validity
	if (!data)
	{
		// Release resources, these are now stored inside OpenGL's texture buffer
		stbi_image_free(data);

		spdlog::warn("Failed to load texture - {}", stbi_failure_reason());
		return false;
	}

	// Generate texture ID
	if(m_ID == GL_INVALID_VALUE)
		glGenTextures(1, &m_ID);
	
	// Set as 2D Texture
	glBindTexture(GL_TEXTURE_2D, m_ID);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Filter);

	// Get texture format based on channels
	GLenum internalFormat = GL_INVALID_ENUM, textureFormat = GL_INVALID_ENUM;
	switch (channelCount)
	{
	case 1:
		textureFormat = GL_R;
		internalFormat = m_HDR ? GL_R16F : GL_R;
		break;
	case 3:
		textureFormat = GL_RGB;
		internalFormat = m_HDR ? GL_RGB16F : GL_RGB;
		break;
	case 4:
		textureFormat = GL_RGBA;
		internalFormat = m_HDR ? GL_RGBA16F : GL_RGBA;
		break;
	}

	// Fill OpenGL texture data with binary data
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, textureFormat, m_HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, data);

	// Release resources, these are now stored inside OpenGL's texture buffer
	stbi_image_free(data);

	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	m_Resolution = glm::ivec2(width, height);

#ifndef NDEBUG
	profileTimer.Stop();
	spdlog::debug("Loaded texture ({}x{}) in {}ms {}", width, height, profileTimer.ElapsedTime().count(), m_HDR ? "[HDR]" : "");
#endif

	return true;
}

bool Texture::GetHDR() { return m_HDR; }
int Texture::GetFilter() { return m_Filter; }
unsigned int Texture::GetID() { return m_ID; }
bool Texture::IsValid() { return m_ID != GL_INVALID_VALUE; }
glm::ivec2& Texture::GetResolution() { return m_Resolution; }

void Texture::Bind(unsigned int index)
{
	if (m_ID == GL_INVALID_VALUE)
		return;
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

#pragma region Managed Binding
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Texture, Load, void, (MonoString* pathRaw, uint64_t* outResourceID, void** outHandle), AquaEngine.Graphics)
{
	char* path = mono_string_to_utf8(pathRaw);
	if (*outResourceID == InvalidResourceID)
		*outResourceID = ResourceID();
	Resource::Load<Texture>(*outResourceID, path);
	*outHandle = Resource::Get<Texture>(*outResourceID);

	if(pathRaw)
		mono_free(path);
}

ADD_MANAGED_METHOD(Texture, Upload, bool, (void* instance, MonoArray* textureDataRaw, bool hdr, int filter), AquaEngine.Graphics)
{
	vector<unsigned char> textureData;
	textureData.resize(mono_array_length(textureDataRaw));
	for (size_t i = 0; i < textureData.size(); i++)
		textureData[i] = mono_array_get(textureDataRaw, unsigned char, i);

	return ((Texture*)instance)->Upload(textureData, hdr, filter);
}

ADD_MANAGED_METHOD(Texture, Bind, void, (void* instance, unsigned int index), AquaEngine.Graphics)
{ ((Texture*)instance)->Bind(index); }

ADD_MANAGED_METHOD(Texture, GetResolution, void, (void* instance, glm::ivec2* outResolution), AquaEngine.Graphics)
{ *outResolution = ((Texture*)instance)->GetResolution(); }

ADD_MANAGED_METHOD(Texture, GetHDR, bool, (void* instance), AquaEngine.Graphics)
{ return ((Texture*)instance)->GetHDR(); }

ADD_MANAGED_METHOD(Texture, GetFilter, int, (void* instance), AquaEngine.Graphics)
{ return ((Texture*)instance)->GetFilter(); }

ADD_MANAGED_METHOD(Texture, IsValid, bool, (void* instance), AquaEngine.Graphics)
{ return ((Texture*)instance)->IsValid(); }

#pragma endregion
