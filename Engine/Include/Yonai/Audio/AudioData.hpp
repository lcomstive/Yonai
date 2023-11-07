#pragma once
#include <miniaudio.h>
#include <Yonai/API.hpp>

namespace Yonai
{
	namespace Components { struct AudioSource; }

	/// <summary>
	/// Resource for an audio clip
	/// </summary>
	struct AudioData
	{
		YonaiAPI AudioData();
		YonaiAPI AudioData(std::vector<unsigned char>& data);

		YonaiAPI void Import(std::vector<unsigned char>& data);

		/// <returns>Length of the clip, in seconds</returns>
		YonaiAPI float GetLength();

	private:
		std::vector<unsigned char> m_Data;

		friend struct Yonai::Components::AudioSource;
	};
}
