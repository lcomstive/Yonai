#pragma once
#include <string>
#include <miniaudio.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/Resource.hpp>

namespace AquaEngine
{
	class SoundMixer
	{
		float m_Volume;

		ResourceID m_Parent;
		ma_sound_group m_Group;

	public:
		std::string Name;

		AquaAPI SoundMixer(std::string name = "Mixer", ResourceID parent = InvalidResourceID);
		AquaAPI ~SoundMixer();

		AquaAPI ma_sound_group* GetHandle();

		AquaAPI float GetVolume();
		AquaAPI void SetVolume(float value);

		/// <summary>
		/// Sets the parent of this mixer, or InvalidResourceID for output to master
		/// </summary>
		AquaAPI void SetParent(ResourceID parent);

		AquaAPI ResourceID GetParent();
	};
}