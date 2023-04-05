#pragma once
#include <miniaudio.h>
#include <AquaEngine/API.hpp>

namespace AquaEngine
{
	namespace Components { struct SoundSource; }

	/// <summary>
	/// Resource for an audio clip
	/// </summary>
	struct Sound
	{
		AquaAPI Sound();
		AquaAPI Sound(std::string& filepath);
		AquaAPI ~Sound();

		AquaAPI void Import(const char* filepath);
		AquaAPI void Import(std::string filepath);

		/// <returns>Length of the clip, in seconds</returns>
		AquaAPI float GetLength();

	private:
		const unsigned int c_Flags = MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE; // | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM;

		ma_sound m_Sound;
		std::string m_FilePath;

		friend struct AquaEngine::Components::SoundSource;
	};
}
