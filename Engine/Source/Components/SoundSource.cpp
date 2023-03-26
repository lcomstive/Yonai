#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Audio/Sound.hpp>
#include <AquaEngine/Components/SoundSource.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Components;

SoundSource::~SoundSource()
{
	// Clear sound
	if (m_Sound != InvalidResourceID)
		ma_sound_uninit(&m_Data);
}

void SoundSource::Play()
{	
	if(IsPlaying() || 				  // Check if already playing
		m_Sound == InvalidResourceID) // or sound not loaded
		return;
	
	ma_sound_start(&m_Data);

	m_State = SoundState::Playing;

	// Reset pause state
	m_PausedFrames = 0;
}

void SoundSource::Pause()
{
	if (m_State != SoundState::Playing)
		return;

	m_State = SoundState::Paused;

	// Store current cursor position
	ma_sound_get_cursor_in_pcm_frames(&m_Data, &m_PausedFrames);

	// Stop playing sound
	ma_sound_stop(&m_Data);
}

void SoundSource::Resume()
{
	if (m_State != SoundState::Paused)
		return;

	m_State = SoundState::Playing;

	// Start sound
	ma_sound_start(&m_Data);

	// Seek to previous position
	ma_sound_seek_to_pcm_frame(&m_Data, m_PausedFrames);
}

void SoundSource::Stop()
{
	// Set state
	m_State = SoundState::Stopped;

	ma_sound_stop(&m_Data);
	ma_sound_seek_to_pcm_frame(&m_Data, 0);

	// Reset pause state
	m_PausedFrames = 0;
}

float SoundSource::GetVolume() { return m_Volume; }
SoundState SoundSource::GetState() { return m_State; }
bool SoundSource::IsPlaying() { return m_State == SoundState::Playing; }
float SoundSource::GetLength()
{
	float length;
	ma_sound_get_length_in_seconds(&m_Data, &length);
	return length;
}

float SoundSource::GetPlayTime()
{
	if (m_State == SoundState::Stopped)
		return 0.0f;

	float position;
	ma_sound_get_cursor_in_seconds(&m_Data, &position);
	return position;
}

void SoundSource::Seek(float seconds)
{
	// Check if sound is initialised
	if (m_Sound == InvalidResourceID)
		return;

	// Ensure value is within bounds
	seconds = std::clamp(seconds, 0.0f, GetLength());

	// Time to seek to
	ma_uint64 pcmTime = (ma_uint64)std::floor(seconds * AudioSystem::GetSampleRate());

	ma_sound_seek_to_pcm_frame(&m_Data, pcmTime);
}

void SoundSource::SetVolume(float volume)
{
	// Ensure volume is at least 0
	m_Volume = (std::max)(volume, 0.0f);

	if(m_Sound != InvalidResourceID)
		ma_sound_set_volume(&m_Data, m_Volume);
}

bool SoundSource::IsLooping() { return m_Looping; }
void SoundSource::SetLooping(bool loop) { ma_sound_set_looping(&m_Data, m_Looping = loop); }

float SoundSource::GetPanning() { return m_Panning; }
void SoundSource::SetPanning(float pan)
{
	m_Panning = std::clamp(pan, -1.0f, 1.0f);
	if(m_Sound != InvalidResourceID) ma_sound_set_pan(&m_Data, m_Panning);
}

float SoundSource::GetPitch() { return m_Pitch; }
void SoundSource::SetPitch(float pitch)
{
	m_Pitch = (std::max)(pitch, 0.0f);
	if (m_Sound != InvalidResourceID) ma_sound_set_pitch(&m_Data, m_Pitch);
}

bool SoundSource::GetSpatialization() { return m_Spatialization; }
void SoundSource::SetSpatialization(bool enable)
{
	m_Spatialization = enable;
	if(m_Sound != InvalidResourceID) ma_sound_set_spatialization_enabled(&m_Data, enable);
}

ResourceID SoundSource::GetSound() { return m_Sound; }
void SoundSource::SetSound(ResourceID id)
{
	// Check if clearing sound
	if (id == InvalidResourceID && m_Sound != InvalidResourceID)
	{
		// Uninitialise
		m_Sound = InvalidResourceID;
		ma_sound_uninit(&m_Data);
		return;
	}

	if(id == m_Sound || // No change, same resource id
		!Resource::IsValidType<Sound>(id)) // Not a valid sound
		return; 

	if(m_Sound != InvalidResourceID)
		ma_sound_uninit(&m_Data); // Release previously loaded sound

	m_Sound = id;
	Sound* sound = Resource::Get<Sound>(m_Sound);

	ma_result result = ma_sound_init_copy(&AudioSystem::s_Engine, &sound->m_Sound, sound->c_Flags, nullptr, &m_Data);
	if(result != MA_SUCCESS)
		spdlog::error("Failed to initialise sound source [{}]", (int)result);

	// Update values //
	SetPitch(m_Pitch);
	SetSpatialization(m_Spatialization);

	if(m_Mixer) // If mixer is not default output
		SetMixer(m_Mixer);
}

ResourceID SoundSource::GetMixer() { return m_Mixer; }
void SoundSource::SetMixer(ResourceID mixer)
{
	m_Mixer = mixer;

	if(m_Sound == InvalidResourceID)
		return; // No sound loaded

	ma_node* output = nullptr;

	// Check for valid mixer
	if(mixer == InvalidResourceID || !Resource::IsValidType<SoundMixer>(mixer))
		output = ma_engine_get_endpoint(AudioSystem::GetEngine());
	else
		output = (ma_node*)Resource::Get<SoundMixer>(mixer)->GetHandle();

	ma_node_attach_output_bus(&m_Data, 0, output, 0);
}

#pragma region Scripting Internal Calls
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(SoundSource, GetSound, unsigned int, (void* instance))
{ return ((SoundSource*)instance)->GetSound(); }

ADD_MANAGED_METHOD(SoundSource, SetSound, void, (void* instance, unsigned int value))
{ ((SoundSource*)instance)->SetSound(value); }

ADD_MANAGED_METHOD(SoundSource, Play, void, (void* instance))
{ ((SoundSource*)instance)->Play(); }

ADD_MANAGED_METHOD(SoundSource, Pause, void, (void* instance))
{ ((SoundSource*)instance)->Pause(); }

ADD_MANAGED_METHOD(SoundSource, Stop, void, (void* instance))
{ ((SoundSource*)instance)->Stop(); }

ADD_MANAGED_METHOD(SoundSource, Seek, void, (void* instance, float seconds))
{ ((SoundSource*)instance)->Seek(seconds); }

ADD_MANAGED_METHOD(SoundSource, IsPlaying, bool, (void* instance))
{ return ((SoundSource*)instance)->IsPlaying(); }

ADD_MANAGED_METHOD(SoundSource, GetState, unsigned int, (void* instance))
{ return (unsigned int)((SoundSource*)instance)->GetState(); }

ADD_MANAGED_METHOD(SoundSource, GetPlayTime, float, (void* instance))
{ return ((SoundSource*)instance)->GetPlayTime(); }

ADD_MANAGED_METHOD(SoundSource, GetLength, float, (void* instance))
{ return ((SoundSource*)instance)->GetLength(); }

ADD_MANAGED_METHOD(SoundSource, GetLooping, bool, (void* instance))
{ return ((SoundSource*)instance)->IsLooping(); }

ADD_MANAGED_METHOD(SoundSource, SetLooping, void, (void* instance, bool value))
{ ((SoundSource*)instance)->SetLooping(value); }

ADD_MANAGED_METHOD(SoundSource, GetSpatialization, bool, (void* instance))
{ return ((SoundSource*)instance)->GetSpatialization(); }

ADD_MANAGED_METHOD(SoundSource, SetSpatialization, void, (void* instance, bool value))
{ ((SoundSource*)instance)->SetSpatialization(value); }

ADD_MANAGED_METHOD(SoundSource, GetMixer, unsigned int, (void* instance))
{ return ((SoundSource*)instance)->GetMixer(); }

ADD_MANAGED_METHOD(SoundSource, SetMixer, void, (void* instance, unsigned int value))
{ ((SoundSource*)instance)->SetMixer(value); }

ADD_MANAGED_METHOD(SoundSource, GetPanning, float, (void* instance))
{ return ((SoundSource*)instance)->GetPanning(); }

ADD_MANAGED_METHOD(SoundSource, SetPanning, void, (void* instance, float value))
{ ((SoundSource*)instance)->SetPanning(value); }

ADD_MANAGED_METHOD(SoundSource, GetVolume, float, (void* instance))
{ return ((SoundSource*)instance)->GetVolume(); }

ADD_MANAGED_METHOD(SoundSource, SetVolume, void, (void* instance, float value))
{ ((SoundSource*)instance)->SetVolume(value); }

ADD_MANAGED_METHOD(SoundSource, GetPitch, float, (void* instance))
{ return ((SoundSource*)instance)->GetPitch(); }

ADD_MANAGED_METHOD(SoundSource, SetPitch, void, (void* instance, float value))
{ ((SoundSource*)instance)->SetPitch(value); }

#pragma endregion
