#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Graphics/Texture.hpp>

#ifndef NDEBUG
#include <AquaEngine/Timer.hpp>
#endif

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Graphics;

Texture::Texture() : m_Path(""), m_ID(GL_INVALID_VALUE) { }

Texture::Texture(string path, bool hdr) : m_Path(path), m_ID(GL_INVALID_VALUE)
{
	GenerateImage(hdr);
}

Texture::~Texture()
{
	if(m_ID != GL_INVALID_VALUE)
		glDeleteTextures(1, &m_ID);
}

void Texture::GenerateImage(bool hdr)
{
	if (m_Path.empty())
	{
		spdlog::warn("Cannot generate texture with empty path");
		return;
	}

#ifndef NDEBUG
	Timer profileTimer;
#endif

	// Read image data
	int width, height, channelCount;
	void* data = nullptr;

	// Flip loaded textures, so OpenGL loads them right way up
	stbi_set_flip_vertically_on_load(true);
	
	if (VFS::Exists(m_Path))
	{
		// Load from virtual filesystem
		vector<unsigned char> fileContents = VFS::Read(m_Path);
		if (!hdr)
			data = stbi_load_from_memory(fileContents.data(), (int)fileContents.size(), &width, &height, &channelCount, 0);
		else
			data = stbi_loadf_from_memory(fileContents.data(), (int)fileContents.size(), &width, &height, &channelCount, 0);
	}
	else // Load from filepath
	{
		if(!hdr)
			data = stbi_load(m_Path.c_str(), &width, &height, &channelCount, 0);
		else
			data = stbi_loadf(m_Path.c_str(), &width, &height, &channelCount, 0);
	}

	// Check for validity
	if (!data)
	{
		// Release resources, these are now stored inside OpenGL's texture buffer
		stbi_image_free(data);

		spdlog::warn("Failed to load '{} - {}", m_Path, stbi_failure_reason());
		return;
	}

	// Generate texture ID
	glGenTextures(1, &m_ID);
	// Set as 2D Texture
	glBindTexture(GL_TEXTURE_2D, m_ID);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Get texture format based on channels
	GLenum internalFormat = GL_INVALID_ENUM, textureFormat = GL_INVALID_ENUM;
	switch (channelCount)
	{
	case 1:
		textureFormat = GL_R;
		internalFormat = hdr ? GL_R16F : GL_R;
		break;
	case 3:
		textureFormat = GL_RGB;
		internalFormat = hdr ? GL_RGB16F : GL_RGB;
		break;
	case 4:
		textureFormat = GL_RGBA;
		internalFormat = hdr ? GL_RGBA16F : GL_RGBA;
		break;
	}

	// Fill OpenGL texture data with binary data
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, textureFormat, hdr ? GL_FLOAT : GL_UNSIGNED_BYTE, data);

	// Release resources, these are now stored inside OpenGL's texture buffer
	stbi_image_free(data);

	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

#ifndef NDEBUG
	profileTimer.Stop();
	spdlog::debug("Loaded texture '{}' in {}ms {}", m_Path, profileTimer.ElapsedTime().count(), hdr ? "[HDR]" : "");
#endif
}

string Texture::GetPath() { return m_Path; }
unsigned int Texture::GetID() { return m_ID; }

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

ADD_MANAGED_METHOD(Texture, Load0, void, (MonoString* path, unsigned int* outResourceID, void** outHandle), AquaEngine.Graphics)
{
	*outResourceID = Resource::Load<Texture>(mono_string_to_utf8(path));
	*outHandle = Resource::Get<Texture>(*outResourceID);
}

ADD_MANAGED_METHOD(Texture, Load1, void, (MonoString* path, MonoString* filePath, bool hdr, unsigned int* outResourceID, void** outHandle), AquaEngine.Graphics)
{
	*outResourceID = Resource::Load<Texture>(mono_string_to_utf8(path), mono_string_to_utf8(filePath), hdr);
	*outHandle = Resource::Get<Texture>(*outResourceID);
}

ADD_MANAGED_METHOD(Texture, Bind, void, (void* instance, unsigned int index), AquaEngine.Graphics)
{ ((Texture*)instance)->Bind(index); }

#pragma endregion