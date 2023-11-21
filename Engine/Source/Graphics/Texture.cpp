#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <Yonai/Graphics/Texture.hpp>

#ifndef NDEBUG
#include <Yonai/Timer.hpp>
#endif

using namespace std;
using namespace Yonai;
using namespace Yonai::Graphics;

Texture::Texture() : m_Filter(GL_LINEAR), m_ID(0), m_Resolution(), m_HDR(false) { }

Texture::~Texture()
{
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
bool Texture::IsValid() { return m_ID != 0; }
glm::ivec2& Texture::GetResolution() { return m_Resolution; }

void Texture::Bind(unsigned int index) {}

#pragma region Managed Binding
#include <Yonai/Resource.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Texture, Load, void, (MonoString* pathRaw, uint64_t* outResourceID, void** outHandle), Yonai.Graphics)
{
	char* path = mono_string_to_utf8(pathRaw);
	if (*outResourceID == InvalidResourceID)
		*outResourceID = ResourceID();
	Resource::Load<Texture>(*outResourceID, path);
	*outHandle = Resource::Get<Texture>(*outResourceID);

	if(pathRaw)
		mono_free(path);
}

ADD_MANAGED_METHOD(Texture, Upload, bool, (void* instance, MonoArray* textureDataRaw, bool hdr, int filter), Yonai.Graphics)
{
	vector<unsigned char> textureData;
	textureData.resize(mono_array_length(textureDataRaw));
	for (size_t i = 0; i < textureData.size(); i++)
		textureData[i] = mono_array_get(textureDataRaw, unsigned char, i);

	return ((Texture*)instance)->Upload(textureData, hdr, filter);
}

ADD_MANAGED_METHOD(Texture, Bind, void, (void* instance, unsigned int index), Yonai.Graphics)
{ ((Texture*)instance)->Bind(index); }

ADD_MANAGED_METHOD(Texture, GetResolution, void, (void* instance, glm::ivec2* outResolution), Yonai.Graphics)
{ *outResolution = ((Texture*)instance)->GetResolution(); }

ADD_MANAGED_METHOD(Texture, GetHDR, bool, (void* instance), Yonai.Graphics)
{ return ((Texture*)instance)->GetHDR(); }

ADD_MANAGED_METHOD(Texture, GetFilter, int, (void* instance), Yonai.Graphics)
{ return ((Texture*)instance)->GetFilter(); }

ADD_MANAGED_METHOD(Texture, IsValid, bool, (void* instance), Yonai.Graphics)
{ return ((Texture*)instance)->IsValid(); }

ADD_MANAGED_METHOD(Texture, Decode, bool, (MonoArray* textureDataRaw, bool hdr, int expectedChannels, MonoArray** output, glm::ivec2* outResolution, int* outChannels), Yonai.Graphics)
{
	if(!textureDataRaw)
		return false;

	vector<unsigned char> textureData;
	textureData.resize(mono_array_length(textureDataRaw));
	for (size_t i = 0; i < textureData.size(); i++)
		textureData[i] = mono_array_get(textureDataRaw, unsigned char, i);

	if (textureData.empty())
	{
		spdlog::warn("Cannot generate texture with empty path");
		return false;
	}

	// Read image data
	int width, height, channelCount;
	void* data = nullptr;
	
	if (!hdr)
		data = stbi_load_from_memory(textureData.data(), (int)textureData.size(), &width, &height, &channelCount, expectedChannels);
	else
		data = stbi_loadf_from_memory(textureData.data(), (int)textureData.size(), &width, &height, &channelCount, expectedChannels);

	if (expectedChannels > 0)
		channelCount = expectedChannels;

	// Check for validity
	if (!data)
	{
		// Release resources, these are now stored inside OpenGL's texture buffer
		stbi_image_free(data);

		spdlog::warn("Failed to load texture - {}", stbi_failure_reason());
		return false;
	}

	*outResolution = glm::ivec2(width, height);

	int dataSize = width * height * channelCount;
	*output = mono_array_new(mono_domain_get(), mono_get_byte_class(), dataSize);
	for(int i = 0; i < dataSize; i++)
		mono_array_set(*output, unsigned char, i, ((unsigned char*)data)[i]);

	*outChannels = channelCount;

	stbi_image_free(data);
	return true;
}

#pragma endregion
