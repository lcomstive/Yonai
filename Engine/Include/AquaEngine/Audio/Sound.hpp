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
		AquaAPI Sound(const std::string& filepath, bool stream = false);
		AquaAPI ~Sound();

		/// <returns>Length of the clip, in seconds</returns>
		AquaAPI float GetLength();

		/// <returns>The sample rate of the loaded sound</returns>
		AquaAPI float GetSampleRate();

	private:
		ma_decoder m_Source;
		float m_SampleRate;

		friend struct AquaEngine::Components::SoundSource;
	};
}