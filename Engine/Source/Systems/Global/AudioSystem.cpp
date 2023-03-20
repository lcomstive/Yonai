#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace AquaEngine::Systems;

#ifndef AQUA_AUDIO_THREADS
#define AQUA_AUDIO_THREADS 4
#endif

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
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

// Set large value by default because we compare this in SetOutputDevice, if they're both '0' then no output device is set
ma_uint32 AudioSystem::s_CurrentDevice = 99999;

// Node Graph //
ma_node_graph AudioSystem::s_NodeGraph;

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

	RefreshDevices();

	// Initialise engine using default device
	SetOutputDevice(s_DefaultDeviceIndex);

	SetupNodeGraph();
}

void AudioSystem::OnEnabled() { s_EngineActive = true; }
void AudioSystem::OnDisabled() { s_EngineActive = false; }

void AudioSystem::Destroy()
{
	// Check if device is initialised
	if (!s_Device.type)
		return;

	spdlog::debug("Audio engine shutting down");

	ma_node_graph_uninit(&s_NodeGraph, nullptr);
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
	// (void)pInput;
	ma_node_graph_read_pcm_frames(&s_NodeGraph, pOutput, frameCount, nullptr);
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
		ma_device_uninit(&s_Device);

	// Get device info
	ma_device_info deviceInfo = s_PlaybackDeviceInfos[deviceIndex];
	
	ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.pDeviceID = &deviceInfo.id;
	deviceConfig.playback.format = ma_format_f32;
	deviceConfig.playback.channels = 0;
	deviceConfig.sampleRate = 48000;
	deviceConfig.dataCallback = AudioDataCallback;

	ma_result result = ma_device_init(&s_Context, &deviceConfig, &s_Device);
	if (result != MA_SUCCESS)
	{
		spdlog::error("Failed to initialise output device [{}]", (int)result);
		return;
	}

	result = ma_device_start(&s_Device);
	if (result != MA_SUCCESS)
	{
		spdlog::error("Failed to start output device [{}]", (int)result);
		ma_device_uninit(&s_Device);
		s_Device = {};
		return;
	}

	s_CurrentDevice = deviceIndex;

	// Notify scripting of change
	s_ScriptClass->Invoke("_OutputDeviceChanged");
}

void AudioSystem::SetupNodeGraph()
{
	ma_node_graph_config config = ma_node_graph_config_init(s_Device.playback.channels);
	ma_node_graph_init(&config, nullptr, &s_NodeGraph);
}

void AudioSystem::GetScriptClass()
{
	if(s_ScriptClass)
		delete s_ScriptClass;
		
	s_ScriptClass = new Class(ScriptEngine::GetCoreAssembly()->GetClassFromName("AquaEngine", "Audio"), nullptr);

	s_ScriptClass->Invoke("_RefreshDevices");
	s_ScriptClass->Invoke("_OutputDeviceChanged");
}

unsigned int AudioSystem::GetSampleRate() { return s_Device.sampleRate; }
unsigned int AudioSystem::GetOutputDevice() { return s_CurrentDevice; }
unsigned int AudioSystem::GetDeviceCount() { return s_PlaybackDeviceCount; }
unsigned int AudioSystem::GetDefaultDevice() { return s_DefaultDeviceIndex; }
const char* AudioSystem::GetDeviceName(unsigned int index)
{
	if(s_PlaybackDeviceCount == 0)
		return nullptr;
	index = (std::min)(index, s_PlaybackDeviceCount - 1);
	return s_PlaybackDeviceInfos[index].name;
}
