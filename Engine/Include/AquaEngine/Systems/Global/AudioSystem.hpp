#pragma once
#define MA_NO_ENGINE
#define MA_NO_RESOURCE_MANAGER

#include <miniaudio.h>
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

// Forward declarations
namespace AquaEngine
{
	struct Sound;
	namespace Scripting { struct Class; }
	namespace Components { struct SoundSource; }
}

namespace AquaEngine::Systems
{
	class AudioSystem : public System
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

		// Node Graph //
		static ma_node_graph s_NodeGraph;

		// Scripting //
		static Scripting::Class* s_ScriptClass;

		static void RefreshDevices();
		static void GetScriptClass();
		static void SetupNodeGraph();
		static void SetupResourceManager();
		static void ReleaseResourceManager();

		static void AudioDataCallback(ma_device*, void*, const void*, ma_uint32);

		friend struct AquaEngine::Sound;
		friend struct AquaEngine::Components::SoundSource;

	public:
		AquaAPI void Init() override;
		AquaAPI void Update() override;
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

		/// <returns>Sample rate for the current output device</returns>
		AquaAPI static unsigned int GetSampleRate();
	};
}