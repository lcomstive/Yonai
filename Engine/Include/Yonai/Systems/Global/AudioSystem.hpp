#pragma once

#include <miniaudio.h>
#include <Yonai/Systems/ScriptSystem.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>

// Forward declarations
namespace Yonai
{
	struct AudioData;
	namespace Scripting { struct Class; }
	namespace Components { struct AudioSource; }
}

namespace Yonai::Systems
{
	class AudioSystem : public ScriptSystem
	{
		/// <summary>
		/// Matches state of AudioSystem::IsEnabled()
		/// </summary>
		static bool s_EngineActive;

		// Devices //
		static ma_device s_Device;
		static ma_context s_Context;
		static ma_uint32 s_CurrentDevice;
		static ma_uint32 s_PlaybackDeviceCount;
		static ma_uint32 s_DefaultDeviceIndex;
		static ma_device_info* s_PlaybackDeviceInfos;

		// Resource Manager //
		static ma_resource_manager s_ResourceManager;

		// Engine //
		static ma_engine s_Engine;

		// Scripting //
		static Scripting::Class* s_ScriptClass;

		static void SetupEngine();
		static void RefreshDevices();
		static void GetScriptClass();
		static void SetupResourceManager();

		static void AudioDataCallback(ma_device*, void*, const void*, ma_uint32);

		friend struct Yonai::AudioData;
		friend struct Yonai::Components::AudioSource;

	public:
		YonaiAPI void Init() override;
		YonaiAPI void Update() override;
		YonaiAPI void Destroy() override;
		YonaiAPI void OnEnabled() override;
		YonaiAPI void OnDisabled() override;

		YonaiAPI static ma_engine* GetEngine();

		/// <summary>
		/// Gets the amount of playback devices available on the system
		/// </summary>
		YonaiAPI static unsigned int GetDeviceCount();

		/// <summary>
		/// Gets index of the default device
		/// </summary>
		YonaiAPI static unsigned int GetDefaultDevice();

		/// <summary>
		/// Gets index of the current output device
		/// </summary>
		YonaiAPI static unsigned int GetOutputDevice();

		/// <summary>
		/// Gets the device name at a given index
		/// </summary>
		YonaiAPI static const char* GetDeviceName(unsigned int index);

		/// <summary>
		/// Sets the audio output device to index
		/// </summary>
		YonaiAPI static void SetOutputDevice(unsigned int index);

		/// <returns>Sample rate for the current output device</returns>
		YonaiAPI static unsigned int GetSampleRate();
	};
}