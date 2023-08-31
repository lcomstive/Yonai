#pragma once
#include <string>
#include <miniaudio.h>
#include <Yonai/API.hpp>
#include <Yonai/Resource.hpp>

namespace Yonai
{
	class SoundMixer
	{
		float m_Volume;

		ResourceID m_Parent;
		ma_sound_group m_Group;

	public:
		std::string Name;

		YonaiAPI SoundMixer(std::string name = "Mixer", ResourceID parent = InvalidResourceID);
		YonaiAPI ~SoundMixer();

		YonaiAPI ma_sound_group* GetHandle();

		YonaiAPI float GetVolume();
		YonaiAPI void SetVolume(float value);

		/// <summary>
		/// Sets the parent of this mixer, or InvalidResourceID for output to master
		/// </summary>
		YonaiAPI void SetParent(ResourceID parent);

		YonaiAPI ResourceID GetParent();
	};
}