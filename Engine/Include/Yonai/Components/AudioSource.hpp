#pragma once
#include <miniaudio.h>
#include <Yonai/API.hpp>
#include <Yonai/ResourceID.hpp>
#include <Yonai/Audio/AudioMixer.hpp>
#include <Yonai/Components/Component.hpp>

namespace Yonai
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

		struct AudioSource : public Component
		{
			YonaiAPI ~AudioSource() override;

			YonaiAPI void Play();
			YonaiAPI void Stop();
			YonaiAPI void Pause();
			YonaiAPI void Resume();

			YonaiAPI bool IsPlaying();
			YonaiAPI SoundState GetState();

			/// <returns>Handle to miniaudio sound. This can be used directly with the miniaudio API for more functionality</returns>
			YonaiAPI ma_sound* GetHandle();

			/// <returns>Amount of time the clip has been played for, in seconds</returns>
			YonaiAPI float GetPlayTime();

			/// <returns>Total length of the sound clip, in seconds</returns>
			YonaiAPI float GetLength();

			YonaiAPI void Seek(float seconds);

			YonaiAPI float GetVolume();
			YonaiAPI void SetVolume(float volume);

			YonaiAPI ResourceID GetSound();
			YonaiAPI void SetSound(ResourceID id);

			YonaiAPI bool IsLooping();
			YonaiAPI void SetLooping(bool loop = true);

			/// <summary>
			/// Gets panning of sound, ranging from -1.0 (fully left) to 1.0 (fully right)
			/// </summary>
			YonaiAPI float GetPanning();

			/// <summary>
			/// Sets panning of sound, ranging from -1.0 (fully left) to 1.0 (fully right)
			/// </summary>
			YonaiAPI void SetPanning(float pan);

			/// <summary>
			/// Gets pitch of sound, higher value gives higher pitch, with a minimum value of 0.
			/// </summary>
			YonaiAPI float GetPitch();

			/// <summary>
			/// Sets pitch of sound, higher value gives higher pitch, with a minimum value of 0.
			/// </summary>
			YonaiAPI void SetPitch(float pan);

			/// <summary>
			/// When enabled, sounds are altered in volume based on position
			/// <summary>
			YonaiAPI bool Get3D();

			/// <summary>
			/// When enabled, sounds are altered in volume based on position
			/// <summary>
			YonaiAPI void Set3D(bool enable);

			/// <summary>
			/// Sets the output mixer, or nullptr for master output
			/// <summary>
			YonaiAPI void SetMixer(ResourceID parent);

			/// <summary>
			/// Gets the output mixer, or nullptr if master output
			/// <summary>
			YonaiAPI ResourceID GetMixer();

		private:
			ma_sound m_Data = {};
			ma_decoder m_Decoder = {};
			ResourceID m_Sound = InvalidResourceID;
			ResourceID m_Mixer = InvalidResourceID;

			bool m_Looping = false;
			float m_Panning = 0.0f;
			float m_Pitch = 1.0f;
			bool m_Is3D = true;

			// Volume of output sound
			float m_Volume = 1.0f;

			// Length of loaded sound
			float m_Length = 0.0f;

			// Where to seek when resuming
			ma_uint64 m_PausedFrames = 0;

			SoundState m_State = SoundState::Stopped;

			/// <summary>
			/// Calls UpdateState(uint state) in managed C# code for this component
			/// </summary>
			void UpdateManagedState();

			friend class Yonai::Systems::AudioSystem;
		};
	}
}