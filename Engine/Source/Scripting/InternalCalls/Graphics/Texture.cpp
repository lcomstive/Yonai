#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace std;

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
