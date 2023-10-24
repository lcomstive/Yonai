#include <spdlog/spdlog.h>
#include <Yonai/Resource.hpp>
#include <Yonai/Audio/Sound.hpp>
#include <Yonai/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Systems;

Sound::Sound() : m_Decoder(), m_Data() { }
Sound::Sound(std::vector<unsigned char>& data) : m_Decoder(), m_Data() { Import(data); }

Sound::~Sound() { ma_decoder_uninit(&m_Decoder); }

float Sound::GetLength()
{
	float seconds = m_Data.empty() ? 0 : m_Data.size() / (float)AudioSystem::s_Engine.sampleRate;
	spdlog::debug("Sound length is {}s", seconds);
	return seconds;
}

void Sound::Import(vector<unsigned char>& data)
{
	if (!m_Data.empty())
		// Unload previously loaded sound
		ma_decoder_uninit(&m_Decoder);

	// Copy data
	m_Data = data;

	// Check if there is a sound to load
	if (data.empty())
		return;

	// Create audio buffer
	ma_decoder_config config = ma_decoder_config_init_default();
	ma_result result = ma_decoder_init_memory(m_Data.data(), m_Data.size(), &config, &m_Decoder);
	if (result != MA_SUCCESS)
		spdlog::error("Failed to decode audio data [{}]", (int)result);
}

#pragma region Scripting Bindings
#include <Yonai/Resource.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Sound, Load, void, (MonoString* path, unsigned long long* outResourceID, void** outHandle))
{
	char* resourcePath = mono_string_to_utf8(path);
	if (*outResourceID == InvalidResourceID)
		*outResourceID = ResourceID(); // If no resource ID, generate one
	Resource::Load<Sound>(*outResourceID, resourcePath);
	*outHandle = Resource::Get<Sound>(*outResourceID);
	mono_free(resourcePath);
}

ADD_MANAGED_METHOD(Sound, Import, void, (void* instance, MonoArray* audioDataRaw))
{
	vector<unsigned char> audioData;
	audioData.resize(mono_array_length(audioDataRaw));
	for (size_t i = 0; i < audioData.size(); i++)
		audioData[i] = mono_array_get(audioDataRaw, unsigned char, i);

	((Sound*)instance)->Import(audioData);
}
#pragma endregion
