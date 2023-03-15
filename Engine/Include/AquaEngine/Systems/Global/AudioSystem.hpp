#pragma once
#include <AquaEngine/Systems/System.hpp>

#if AQUA_ENABLE_AUDIO
#include <miniaudio.h>
#endif

namespace AquaEngine::Systems
{
	class AudioSystem : public System
	{
#if AQUA_ENABLE_AUDIO
		bool m_EngineInitialised = false;

		ma_device m_Device;
		ma_engine m_Engine;
		ma_context m_Context;
		ma_device_info* m_PlaybackDeviceInfos;
		ma_uint32 m_PlaybackDeviceCount;
		ma_uint32 m_DefaultDeviceIndex;
#endif

	public:
		AquaAPI void Init() override;
		AquaAPI void Update() override;
		AquaAPI void Destroy() override;
		AquaAPI void OnEnabled() override;

		/// <summary>
		/// Gets the amount of playback devices available on the system
		/// </summary>
		AquaAPI unsigned int GetDeviceCount();

		/// <summary>
		/// Gets index of the default device
		/// </summary>
		AquaAPI unsigned int GetDefaultDevice();

		/// <summary>
		/// Gets the device name at a given index
		/// </summary>
		AquaAPI const char* GetDeviceName(unsigned int index);

		/// <summary>
		/// Sets the audio output device to index
		/// </summary>
		AquaAPI void SetOutputDevice(unsigned int index);
	};
}