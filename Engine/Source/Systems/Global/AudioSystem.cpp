#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace AquaEngine::Systems;

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Class.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Components/SoundSource.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

bool AudioSystem::s_EngineActive = false;
Class* AudioSystem::s_ScriptClass = nullptr;

// Devices //
ma_device AudioSystem::s_Device;
ma_context AudioSystem::s_Context;
ma_uint32 AudioSystem::s_DefaultDeviceIndex = 0;
ma_uint32 AudioSystem::s_PlaybackDeviceCount = 0;
ma_device_info* AudioSystem::s_PlaybackDeviceInfos = nullptr;

// Resource Manager //
ma_resource_manager AudioSystem::s_ResourceManager = {};

// Engine //
ma_engine AudioSystem::s_Engine = {};

// Set large value by default because we compare this in SetOutputDevice, if they're both '0' then no output device is set
ma_uint32 AudioSystem::s_CurrentDevice = 99999;

void AudioSystem::Init()
{
	spdlog::debug("Audio engine initialising");

	if (ma_context_init(nullptr, 0, nullptr, &s_Context) != MA_SUCCESS)
	{
		spdlog::error("Failed to initialise audio - Could not initialise context");
		return;
	}

	GetScriptClass();
	ScriptEngine::AddReloadCallback(GetScriptClass);

	SetupResourceManager();

	RefreshDevices();

	// Initialise engine using default device
	SetOutputDevice(s_DefaultDeviceIndex);
}

void AudioSystem::OnEnabled() { s_EngineActive = true; }
void AudioSystem::OnDisabled() { s_EngineActive = false; }

void AudioSystem::Destroy()
{
	// Check if device is initialised
	if (!s_Device.type)
		return;

	spdlog::debug("Audio engine shutting down");

	ma_engine_uninit(&s_Engine);
	ma_device_uninit(&s_Device);
	ma_context_uninit(&s_Context);

	delete s_ScriptClass;
}

void AudioSystem::Update()
{
	static SceneSystem* sceneSystem = nullptr;

	if (!sceneSystem)
		sceneSystem = SystemManager::Global()->Get<SceneSystem>();

	vector<World*> scenes = sceneSystem->GetActiveScenes();
	for (World* scene : scenes)
	{
		vector<SoundSource*> soundSources = scene->GetComponents<SoundSource>();
		for (SoundSource* source : soundSources)
		{
			// Check if finished playing
			if (source->IsPlaying() &&
				source->GetPlayTime() >= source->GetLength())
				source->Stop();

			Transform* transform = source->Entity.GetComponent<Transform>();
			if(transform)
				ma_sound_set_position(&source->m_Data, transform->Position.x, transform->Position.y, transform->Position.z);
		}
	}
}

void AudioSystem::RefreshDevices()
{
	// Enumerate audio devices
	if (ma_context_get_devices(
		&s_Context,
		&s_PlaybackDeviceInfos,
		&s_PlaybackDeviceCount,
		nullptr, nullptr) != MA_SUCCESS)
	{
		spdlog::error("Failed to enumerate device info");
		return;
	}

	spdlog::trace("Playback Devices: ");
	for (ma_uint32 i = 0; i < s_PlaybackDeviceCount; i++)
	{
		if (s_PlaybackDeviceInfos[i].isDefault)
			s_DefaultDeviceIndex = i;

		spdlog::trace("\t{} {}",
			s_PlaybackDeviceInfos[i].name,
			s_PlaybackDeviceInfos[i].isDefault ? "[Default]" : "");
	}

	// Inform scripting side of changes
	s_ScriptClass->Invoke("_RefreshDevices");
}

void AudioSystem::AudioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	(void)pInput;
	ma_engine_read_pcm_frames(&s_Engine, pOutput, frameCount, nullptr);
}

void AudioDeviceCallback(const ma_device_notification* notification)
{
	spdlog::debug("Audio device callback: {}", (int)notification->type);
}

void AudioSystem::SetOutputDevice(unsigned int deviceIndex)
{
	deviceIndex = (std::min)(deviceIndex, s_PlaybackDeviceCount);
	if (deviceIndex == s_CurrentDevice)
		return; // No change

	// Check if device has been initialised
	if (s_Device.type)
	{
		ma_engine_uninit(&s_Engine);
		ma_device_uninit(&s_Device);
	}

	// Get device info
	ma_device_info deviceInfo = s_PlaybackDeviceInfos[deviceIndex];
	
	ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.pDeviceID = &deviceInfo.id;
	deviceConfig.playback.format = s_ResourceManager.config.decodedFormat;
	deviceConfig.playback.channels = 0;
	deviceConfig.sampleRate = s_ResourceManager.config.decodedSampleRate;
	deviceConfig.dataCallback = AudioDataCallback;

	ma_result result = ma_device_init(&s_Context, &deviceConfig, &s_Device);
	if (result != MA_SUCCESS)
	{
		spdlog::error("Failed to initialise output device [{}]", (int)result);
		return;
	}

	SetupEngine();

	s_CurrentDevice = deviceIndex;

	// Notify scripting of change
	s_ScriptClass->Invoke("_OutputDeviceChanged");
}

void AudioSystem::SetupEngine()
{
	ma_engine_config config = ma_engine_config_init();
	config.pDevice = &s_Device;
	config.pResourceManager = &s_ResourceManager;
	ma_engine_init(&config, &s_Engine);
}

void AudioSystem::GetScriptClass()
{
	if(s_ScriptClass)
		delete s_ScriptClass;
		
	s_ScriptClass = new Class(ScriptEngine::GetCoreAssembly()->GetClassFromName("AquaEngine", "Audio"), nullptr);

	s_ScriptClass->Invoke("_RefreshDevices");
	s_ScriptClass->Invoke("_OutputDeviceChanged");
}

void AudioSystem::SetupResourceManager()
{
	ma_resource_manager_config config = ma_resource_manager_config_init();
	config.decodedFormat = ma_format_f32;
	config.decodedSampleRate = 48000;
	config.decodedChannels = 0; // Use native channel count

	ma_result result = ma_resource_manager_init(&config, &s_ResourceManager);
	if(result != MA_SUCCESS)
		spdlog::error("Failed to initialise audio resource manager [{}]", (int)result);
}

unsigned int AudioSystem::GetSampleRate() { return s_Device.sampleRate; }
unsigned int AudioSystem::GetOutputDevice() { return s_CurrentDevice; }
unsigned int AudioSystem::GetDeviceCount() { return s_PlaybackDeviceCount; }
unsigned int AudioSystem::GetDefaultDevice() { return s_DefaultDeviceIndex; }
ma_engine* AudioSystem::GetEngine() { return &s_Engine; }
const char* AudioSystem::GetDeviceName(unsigned int index)
{
	if(s_PlaybackDeviceCount == 0)
		return nullptr;
	index = (std::min)(index, s_PlaybackDeviceCount - 1);
	return s_PlaybackDeviceInfos[index].name;
}
