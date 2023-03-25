#pragma once
#include <string>
#include <miniaudio.h>
#include <AquaEngine/API.hpp>

namespace AquaEngine
{
	class SoundMixer
	{
		float m_Volume;

		ma_sound_group m_Group;

	public:
		std::string Name;

		AquaAPI SoundMixer(std::string name = "Mixer", SoundMixer* parent = nullptr);
		AquaAPI ~SoundMixer();

		AquaAPI ma_sound_group* GetHandle();

		AquaAPI float GetVolume();
		AquaAPI void SetVolume(float value);

		/// <summary>
		/// Sets the parent of this mixer, or nullptr for output to playback device
		/// </summary>
		AquaAPI void SetParent(SoundMixer* other);
	};
}