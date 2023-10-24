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
		YonaiAPI Sound(std::vector<unsigned char>& data);
		YonaiAPI ~Sound();

		YonaiAPI void Import(std::vector<unsigned char>& data);

		/// <returns>Length of the clip, in seconds</returns>
		YonaiAPI float GetLength();

	private:
		ma_decoder m_Decoder;
		std::vector<unsigned char> m_Data;

		friend struct Yonai::Components::SoundSource;
	};
}
