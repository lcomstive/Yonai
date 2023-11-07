#include <Yonai/Systems/Global/AudioSystem.hpp>

using namespace Yonai::Systems;

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include <spdlog/spdlog.h>
#include <Yonai/Scripting/Class.hpp>
#include <Yonai/Components/Transform.hpp>
#include <Yonai/Components/AudioSource.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Components/AudioListener.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::IO;
using namespace Yonai::Scripting;
using namespace Yonai::Components;

bool AudioSystem::s_EngineActive = false;
Class* AudioSystem::s_ScriptClass = nullptr;

typedef void (*AudioSourceUpdateManagedStateFn)(MonoObject*, unsigned int, MonoException**);
AudioSourceUpdateManagedStateFn AudioSourceUpdateManagedState;

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
	ScriptSystem::Init();

	spdlog::debug("AudioData engine initialising");

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

void AudioSystem::OnEnabled()
{
	s_EngineActive = true;
	ScriptSystem::OnEnabled();
}

void AudioSystem::OnDisabled()
{
	s_EngineActive = false;
	ScriptSystem::OnDisabled();
}

void AudioSystem::Destroy()
{
	ScriptSystem::Destroy();

	// Check if device is initialised
	if (!s_Device.type)
		return;

	spdlog::debug("AudioData engine shutting down");

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

	int listenerIndex = 0;

	// Audio Source
	vector<World*> scenes = sceneSystem->GetActiveScenes();
	for (World* scene : scenes)
	{
		vector<AudioSource*> sources = scene->GetComponents<AudioSource>();
		for (AudioSource* source : sources)
		{
			// Check if finished playing
			if (source->IsPlaying() &&
				source->GetPlayTime() >= source->GetLength())
				source->Stop();

			Transform* transform = source->Entity.GetComponent<Transform>();
			if (!transform)
				continue;

			glm::vec3 pos = transform->GetGlobalPosition();
			ma_sound_set_position(&source->m_Data, pos.x, pos.y, pos.z);
		}

		vector<AudioListener*> listeners = scene->GetComponents<AudioListener>();
		for (AudioListener* listener : listeners)
		{
			Transform* transform = listener->Entity.GetComponent<Transform>();
			if (!transform)
				continue;

			glm::vec3 pos = transform->GetGlobalPosition();
			glm::vec3 forward = transform->GlobalForward();
			ma_engine_listener_set_position(&s_Engine, listenerIndex, pos.x, pos.y, pos.z);
			ma_engine_listener_set_direction(&s_Engine, listenerIndex, forward.x, forward.y, forward.z);

			listenerIndex++;
		}
	}

	ScriptSystem::Update();
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
	spdlog::debug("AudioData device callback: {}", (int)notification->type);
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
		
	s_ScriptClass = new Class(ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai", "Audio"), nullptr);

	s_ScriptClass->Invoke("_RefreshDevices");
	s_ScriptClass->Invoke("_OutputDeviceChanged");

	// Get AudioSource.UpdateState unmanaged thunk
	MonoClass* klass = Scripting::ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai", "AudioSource");
	MonoMethod* method = mono_class_get_method_from_name(klass, "UpdateState", 1);

	AudioSourceUpdateManagedState = method ? (AudioSourceUpdateManagedStateFn)mono_method_get_unmanaged_thunk(method) : nullptr;
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
