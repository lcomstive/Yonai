#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace AquaEngine::Systems;

#if AQUA_ENABLE_AUDIO
#if AQUA_PLATFORM_APPLE
#define MA_NO_RUNTIME_LINKING
#endif

#include <AquaEngine/IO/VFS.hpp> // TEMP //

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include <spdlog/spdlog.h>

using namespace std;
using namespace AquaEngine::IO;

void AudioSystem::Init()
{
	spdlog::debug("Audio engine initialising");

	// Enumerate audio devices
	if (ma_context_init(nullptr, 0, nullptr, &m_Context) != MA_SUCCESS)
	{
		spdlog::error("Failed to initialise audio - Could not initialise context");
		return;
	}

	if(ma_context_get_devices(
		&m_Context,
		&m_PlaybackDeviceInfos,
		&m_PlaybackDeviceCount,
		nullptr, nullptr) != MA_SUCCESS)
	{
		spdlog::error("Failed to initialise audio - Could not get device info");
		return;
	}

	spdlog::trace("Playback Devices: ");
	for (ma_uint32 i = 0; i < m_PlaybackDeviceCount; i++)
	{
		if (m_PlaybackDeviceInfos[i].isDefault)
			m_DefaultDeviceIndex = i;

		spdlog::trace("\t{} {}",
			m_PlaybackDeviceInfos[i].name,
			m_PlaybackDeviceInfos[i].isDefault ? "[Default]" : "");
	}

	// Initialise engine using default device
	SetOutputDevice(m_DefaultDeviceIndex);
}

ma_sound sound, sound2;
void AudioSystem::OnEnabled()
{
	// Test Sound //
	ma_fence fence;
	string audioPath = VFS::GetAbsolutePath("assets://Audio/Lifelike.mp3");
	string audioPath2 = VFS::GetAbsolutePath("assets://Audio/Bell.mp3");

	if (ma_sound_init_from_file(&m_Engine, audioPath.c_str(), MA_SOUND_FLAG_STREAM, nullptr, nullptr, &sound) != MA_SUCCESS)
		spdlog::error("Failed to load file '{}'", audioPath.c_str());
	if(ma_sound_init_from_file(&m_Engine, audioPath2.c_str(), 0, nullptr, nullptr, &sound2) != MA_SUCCESS)
		spdlog::error("Failed to load file '{}'", audioPath2.c_str());

	ma_sound_start(&sound);
}

#include <AquaEngine/Time.hpp>
void AudioSystem::Update()
{
	static float time = 0;
	time += Time::DeltaTime();

	if (time >= 1.0f)
	{
		ma_sound_start(&sound2);
		time -= 1.0f;
	}
}

void AudioSystem::Destroy()
{
	if (!m_EngineInitialised)
		return;

	spdlog::debug("Audio engine shutting down");

	ma_device_uninit(&m_Device);
	ma_context_uninit(&m_Context);
	ma_engine_uninit(&m_Engine);
}

void AudioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	(void)pInput;

	/*
	Since we're managing the underlying device ourselves, we need to read from the engine directly.
	To do this we need access to the ma_engine object which we passed in to the user data. One
	advantage of this is that you could do your own audio processing in addition to the engine's
	standard processing.
	*/
	ma_engine_read_pcm_frames((ma_engine*)pDevice->pUserData, pOutput, frameCount, NULL);
}

void AudioSystem::SetOutputDevice(unsigned int deviceIndex)
{
	if(m_EngineInitialised)
		ma_engine_uninit(&m_Engine);

	deviceIndex = (std::min)(deviceIndex, m_PlaybackDeviceCount);
	
	// Get device info
	ma_device_info deviceInfo = m_PlaybackDeviceInfos[deviceIndex];
	
	ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.pDeviceID = &deviceInfo.id;
	deviceConfig.playback.format = ma_format_f32;
	deviceConfig.playback.channels = 0;
	deviceConfig.sampleRate = 48000;
	deviceConfig.dataCallback = AudioDataCallback;
	deviceConfig.pUserData = &m_Engine;

	if (ma_device_init(&m_Context, &deviceConfig, &m_Device) != MA_SUCCESS)
	{
		spdlog::error("Failed to set output device - Device init failure");
		return;
	}

	// Configure audio engine
	ma_engine_config engineConfig = ma_engine_config_init();
	engineConfig.pPlaybackDeviceID = &deviceInfo.id;
	engineConfig.pDevice = &m_Device;

	if (ma_engine_init(&engineConfig, &m_Engine) != MA_SUCCESS)
	{
		spdlog::error("Failed to set output device - Engine init failure");
		ma_device_uninit(&m_Device);
		return;
	}

	m_EngineInitialised = true;
}

unsigned int AudioSystem::GetDeviceCount() { return m_PlaybackDeviceCount; }
unsigned int AudioSystem::GetDefaultDevice() { return m_DefaultDeviceIndex; }
const char* AudioSystem::GetDeviceName(unsigned int index)
{
	if(m_PlaybackDeviceCount == 0)
		return nullptr;
	index = (std::min)(index, m_PlaybackDeviceCount - 1);
	return m_PlaybackDeviceInfos[index].name;
}

#else // Audio disabled
void AudioSystem::Init() { this->Enable(false); }
void AudioSystem::Update() {}
void AudioSystem::Destroy() {}

void AudioSystem::SetOutputDevice(unsigned int) { }
unsigned int AudioSystem::GetDeviceCount() { return 0; }
unsigned int AudioSystem::GetDefaultDevice() { return 0; }
const char* AudioSystem::GetDeviceName(unsigned int) { return nullptr; }
#endif
