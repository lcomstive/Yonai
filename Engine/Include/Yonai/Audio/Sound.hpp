#pragma once
#include <miniaudio.h>
#include <Yonai/API.hpp>

namespace Yonai
{
	namespace Components { struct SoundSource; }

	/// <summary>
	/// Resource for an audio clip
	/// </summary>
	struct Sound
	{
		YonaiAPI Sound();
		YonaiAPI Sound(std::string& filepath);
		YonaiAPI ~Sound();

		YonaiAPI void Import(const char* filepath);
		YonaiAPI void Import(std::string filepath);

		/// <returns>Length of the clip, in seconds</returns>
		YonaiAPI float GetLength();

	private:
		const unsigned int c_Flags = MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE; // | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM;

		ma_sound m_Sound;
		std::string m_FilePath;

		friend struct Yonai::Components::SoundSource;
	};
}
