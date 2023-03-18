#pragma once
#include <miniaudio.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine
{
	// Forward declaration
	namespace Systems { class AudioSystem; }

	namespace Components
	{
		enum class SoundState : unsigned int
		{
			Stopped = 0,
			Playing,
			Paused
		};

		struct SoundSource : public Component
		{
			ResourceID SoundClip;

			AquaAPI void Play();
			AquaAPI void Stop();
			AquaAPI void Pause();
			AquaAPI void Resume();

			AquaAPI bool IsPlaying();
			AquaAPI SoundState GetState();

			/// <returns>Amount of time the clip has been played for, in seconds</returns>
			AquaAPI float GetPlayTime();

			/// <returns>Total length of the sound clip, in seconds</returns>
			AquaAPI float GetLength();

		private:
			// Sample rate of the sound source
			float m_SampleRate = 0;

			// Total PCM frames
			ma_uint64 m_TotalFrames = 0;

			ma_data_source_node m_Node;
			SoundState m_State = SoundState::Stopped;
			ma_decoder m_Data;
		};
	}
}