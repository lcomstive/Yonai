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

			AquaAPI void Seek(float seconds);

			AquaAPI float GetVolume();
			AquaAPI void SetVolume(float volume);

			AquaAPI ResourceID GetSound();
			AquaAPI void SetSound(ResourceID id);

			AquaAPI bool IsLooping();
			AquaAPI void SetLooping(bool loop = true);

			/// <summary>
			/// Gets panning of sound, ranging from -1.0 (fully left) to 1.0 (fully right)
			/// </summary>
			AquaAPI float GetPanning();

			/// <summary>
			/// Sets panning of sound, ranging from -1.0 (fully left) to 1.0 (fully right)
			/// </summary>
			AquaAPI void SetPanning(float pan);

			/// <summary>
			/// When enabled, sounds are altered in volume based on position
			/// <summary>
			AquaAPI bool GetSpatialization();

			/// <summary>
			/// When enabled, sounds are altered in volume based on position
			/// <summary>
			AquaAPI void SetSpatialization(bool enable);

		private:
			ma_sound m_Data;
			ResourceID m_Sound;

			bool m_Looping = false;
			float m_Panning = 0.0f;
			bool m_Spatialization = true;

			// Volume of output sound
			float m_Volume = 1.0f;

			// Where to seek when resuming
			ma_uint64 m_PausedFrames = 0;

			SoundState m_State = SoundState::Stopped;

			friend class AquaEngine::Systems::AudioSystem;
		};
	}
}