#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Audio/Sound.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;

Sound::Sound(const string& filepath, bool stream) : m_Source()
{
	string assetPath = VFS::GetAbsolutePath(filepath);

	// Set up flags for miniaudio
	unsigned int flags = 0; // MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC;
	if (stream) // Don't load entire file into memory, stream in instead
		flags |= MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM;
	else // Decode entire file into memory
		flags |= MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE;

	auto device = AudioSystem::s_Device;
	auto playback = device.playback;
	m_SampleRate = (float)device.sampleRate;
	ma_decoder_config config = ma_decoder_config_init(playback.format, playback.channels, device.sampleRate);
	ma_result result = ma_decoder_init_file(assetPath.c_str(), &config, &m_Source);

	if (result != MA_SUCCESS)
		spdlog::warn("Failed to create sound for '{}'", filepath.c_str());
}

Sound::~Sound() { ma_decoder_uninit(&m_Source); }

float Sound::GetLength()
{
	ma_uint64 output;
	ma_decoder_get_length_in_pcm_frames(&m_Source, &output);
	return output / m_SampleRate; // Convert from frames to seconds
}

float Sound::GetSampleRate() { return m_SampleRate; }

#pragma region Scripting Bindings
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Sound, Load, void, (MonoString* path, MonoString* audioFilePath, bool stream, unsigned int* outResourceID, void** outHandle))
{
	*outResourceID = Resource::Load<Sound>(mono_string_to_utf8(path), mono_string_to_utf8(audioFilePath), stream);
	*outHandle = Resource::Get<Sound>(*outResourceID);
}
#pragma endregion