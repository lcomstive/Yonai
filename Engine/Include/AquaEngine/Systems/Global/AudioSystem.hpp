#pragma once
#include <AquaEngine/Systems/System.hpp>

#if AQUA_ENABLE_AUDIO
#include <miniaudio.h>
namespace AquaEngine::Scripting { struct Class; } // Forward declaration
#endif


namespace AquaEngine::Systems
{
	class AudioSystem : public System
	{
#if AQUA_ENABLE_AUDIO
		static bool m_EngineInitialised;

		/// <summary>
		/// Matches state of AudioSystem::IsEnabled()
		/// </summary>
		static bool m_EngineActive;

		static ma_device m_Device;
		static ma_engine m_Engine;
		static ma_context m_Context;
		static ma_uint32 m_CurrentDevice;
		static ma_uint32 m_PlaybackDeviceCount;
		static ma_uint32 m_DefaultDeviceIndex;
		static ma_device_info* m_PlaybackDeviceInfos;

		static Scripting::Class* m_ScriptClass;

		static void RefreshDevices();
#endif

	public:
		AquaAPI void Init() override;
		AquaAPI void Destroy() override;
		AquaAPI void OnEnabled() override;
		AquaAPI void OnDisabled() override;

		/// <summary>
		/// Gets the amount of playback devices available on the system
		/// </summary>
		AquaAPI static unsigned int GetDeviceCount();

		/// <summary>
		/// Gets index of the default device
		/// </summary>
		AquaAPI static unsigned int GetDefaultDevice();

		/// <summary>
		/// Gets index of the current output device
		/// </summary>
		AquaAPI static unsigned int GetOutputDevice();

		/// <summary>
		/// Gets the device name at a given index
		/// </summary>
		AquaAPI static const char* GetDeviceName(unsigned int index);

		/// <summary>
		/// Sets the audio output device to index
		/// </summary>
		AquaAPI static void SetOutputDevice(unsigned int index);

		/// <summary>
		/// Plays a sound once, loaded from path
		/// </summary>
		AquaAPI static void PlayOnce(std::string& path);

		/// <summary>
		/// Plays a sound once, loaded from path
		/// </summary>
		AquaAPI static void PlayOnce(const char* path);
	};
}