#pragma once
#include <miniaudio.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Audio/SoundMixer.hpp>
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
			AquaAPI ~SoundSource() override;

			AquaAPI void Play();
			AquaAPI void Stop();
			AquaAPI void Pause();
			AquaAPI void Resume();

			AquaAPI bool IsPlaying();
			AquaAPI SoundState GetState();

			/// <returns>Handle to miniaudio sound. This can be used directly with the miniaudio API for more functionality</returns>
			AquaAPI ma_sound* GetHandle();

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
			/// Gets pitch of sound, higher value gives higher pitch, with a minimum value of 0.
			/// </summary>
			AquaAPI float GetPitch();

			/// <summary>
			/// Sets pitch of sound, higher value gives higher pitch, with a minimum value of 0.
			/// </summary>
			AquaAPI void SetPitch(float pan);

			/// <summary>
			/// When enabled, sounds are altered in volume based on position
			/// <summary>
			AquaAPI bool GetSpatialization();

			/// <summary>
			/// When enabled, sounds are altered in volume based on position
			/// <summary>
			AquaAPI void SetSpatialization(bool enable);

			/// <summary>
			/// Sets the output mixer, or nullptr for master output
			/// <summary>
			AquaAPI void SetMixer(ResourceID parent);

			/// <summary>
			/// Gets the output mixer, or nullptr if master output
			/// <summary>
			AquaAPI ResourceID GetMixer();

		private:
			ma_sound m_Data = {};
			ResourceID m_Sound = InvalidResourceID;
			ResourceID m_Mixer = InvalidResourceID;

			bool m_Looping = false;
			float m_Panning = 0.0f;
			float m_Pitch = 1.0f;
			bool m_Spatialization = true;

			// Volume of output sound
			float m_Volume = 1.0f;

			// Where to seek when resuming
			ma_uint64 m_PausedFrames = 0;

			SoundState m_State = SoundState::Stopped;

			/// <summary>
			/// Calls UpdateState(uint state) in managed C# code for this component
			/// </summary>
			void UpdateManagedState();

			friend class AquaEngine::Systems::AudioSystem;
		};
	}
}