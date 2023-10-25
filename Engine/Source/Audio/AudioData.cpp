#include <spdlog/spdlog.h>
#include <Yonai/Resource.hpp>
#include <Yonai/Audio/AudioData.hpp>
#include <Yonai/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Systems;

AudioData::AudioData() : m_Data() { }
AudioData::AudioData(std::vector<unsigned char>& data) : m_Data() { Import(data); }

float AudioData::GetLength()
{
	float seconds = m_Data.empty() ? 0 : m_Data.size() / (float)AudioSystem::s_Device.playback.internalSampleRate;
	spdlog::debug("AudioData length is {}s", seconds);
	return seconds;
}

void AudioData::Import(vector<unsigned char>& data)
{
	// Copy data
	m_Data = data;
}

#pragma region Scripting Bindings
#include <Yonai/Resource.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(AudioData, Load, void, (MonoString* path, unsigned long long* outResourceID, void** outHandle))
{
	char* resourcePath = mono_string_to_utf8(path);
	if (*outResourceID == InvalidResourceID)
		*outResourceID = ResourceID(); // If no resource ID, generate one
	Resource::Load<AudioData>(*outResourceID, resourcePath);
	*outHandle = Resource::Get<AudioData>(*outResourceID);
	mono_free(resourcePath);
}

ADD_MANAGED_METHOD(AudioData, Import, void, (void* instance, MonoArray* audioDataRaw))
{
	vector<unsigned char> audioData;
	audioData.resize(mono_array_length(audioDataRaw));
	for (size_t i = 0; i < audioData.size(); i++)
		audioData[i] = mono_array_get(audioDataRaw, unsigned char, i);

	((AudioData*)instance)->Import(audioData);
}
#pragma endregion
