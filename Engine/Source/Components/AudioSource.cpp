#include <Yonai/Resource.hpp>
#include <Yonai/Audio/AudioData.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Components/AudioSource.hpp>
#include <Yonai/Systems/Global/AudioSystem.hpp>

using namespace Yonai;
using namespace Yonai::Systems;
using namespace Yonai::Components;

extern void (*AudioSourceUpdateManagedState)(MonoObject*, unsigned int, MonoException**);

AudioSource::~AudioSource()
{
	// Clear sound
	if (m_Sound != InvalidResourceID)
		ma_sound_uninit(&m_Data);
}

void AudioSource::Play()
{	
	if(IsPlaying() || 				  // Check if already playing
		m_Sound == InvalidResourceID) // or sound not loaded
		return;
	
	ma_sound_start(&m_Data);

	m_State = SoundState::Playing;

	// Reset pause state
	m_PausedFrames = 0;

	UpdateManagedState();
}

void AudioSource::Pause()
{
	if (m_State != SoundState::Playing)
		return; // No change

	m_State = SoundState::Paused;

	// Store current cursor position
	ma_sound_get_cursor_in_pcm_frames(&m_Data, &m_PausedFrames);

	// Stop playing sound
	ma_sound_stop(&m_Data);

	UpdateManagedState();
}

void AudioSource::Resume()
{
	if (m_State != SoundState::Paused)
		return; // No change

	m_State = SoundState::Playing;

	// Start sound
	ma_sound_start(&m_Data);

	// Seek to previous position
	ma_sound_seek_to_pcm_frame(&m_Data, m_PausedFrames);

	UpdateManagedState();
}

void AudioSource::Stop()
{
	if (m_State == SoundState::Stopped)
		return; // No change

	// Set state
	m_State = SoundState::Stopped;

	ma_sound_stop(&m_Data);
	ma_sound_seek_to_pcm_frame(&m_Data, 0);

	// Reset pause state
	m_PausedFrames = 0;

	UpdateManagedState();
}

float AudioSource::GetVolume() { return m_Volume; }
SoundState AudioSource::GetState() { return m_State; }
bool AudioSource::IsPlaying() { return m_State == SoundState::Playing; }
float AudioSource::GetLength() { return m_Length; }

float AudioSource::GetPlayTime()
{
	if (m_State == SoundState::Stopped)
		return 0.0f;

	float position;
	ma_sound_get_cursor_in_seconds(&m_Data, &position);
	return position;
}

void AudioSource::Seek(float seconds)
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

void AudioSource::SetVolume(float volume)
{
	// Ensure volume is at least 0
	m_Volume = (std::max)(volume, 0.0f);

	if(m_Sound != InvalidResourceID)
		ma_sound_set_volume(&m_Data, m_Volume);
}

bool AudioSource::IsLooping() { return m_Looping; }
void AudioSource::SetLooping(bool loop) { ma_sound_set_looping(&m_Data, m_Looping = loop); }

float AudioSource::GetPanning() { return m_Panning; }
void AudioSource::SetPanning(float pan)
{
	m_Panning = std::clamp(pan, -1.0f, 1.0f);
	if(m_Sound != InvalidResourceID) ma_sound_set_pan(&m_Data, m_Panning);
}

float AudioSource::GetPitch() { return m_Pitch; }
void AudioSource::SetPitch(float pitch)
{
	m_Pitch = (std::max)(pitch, 0.0f);
	if (m_Sound != InvalidResourceID) ma_sound_set_pitch(&m_Data, m_Pitch);
}

bool AudioSource::GetSpatialization() { return m_Spatialization; }
void AudioSource::SetSpatialization(bool enable)
{
	m_Spatialization = enable;
	if(m_Sound != InvalidResourceID) ma_sound_set_spatialization_enabled(&m_Data, enable);
}

ResourceID AudioSource::GetSound() { return m_Sound; }
void AudioSource::SetSound(ResourceID id)
{
	// Check if clearing sound
	if (id == InvalidResourceID && m_Sound != InvalidResourceID)
	{
		// Uninitialise
		m_Sound = InvalidResourceID;
		ma_sound_uninit(&m_Data);
		ma_decoder_uninit(&m_Decoder);
		return;
	}

	if(id == m_Sound || // No change, same resource id
		!Resource::IsValidType<AudioData>(id)) // Not a valid sound
		return; 

	if (m_Sound != InvalidResourceID)
	{
		// Release previously loaded sound
		ma_sound_uninit(&m_Data);
		ma_decoder_uninit(&m_Decoder);
	}

	m_Sound = id;
	AudioData* sound = Resource::Get<AudioData>(m_Sound);
	m_Length = 0;

	if (!sound || sound->GetLength() <= 0.0f)
	{
		spdlog::error("Failed to initialise sound source - invalid sound");
		m_Sound = InvalidResourceID;
		return;
	}
	
	ma_decoder_config config = ma_decoder_config_init_default();
	ma_result result = ma_decoder_init_memory(sound->m_Data.data(), sound->m_Data.size(), &config, &m_Decoder);
	if (result != MA_SUCCESS)
		spdlog::error("Failed to decode audio data [{}]", (int)result);

	result = ma_sound_init_from_data_source(
		&AudioSystem::s_Engine,
		&m_Decoder,
		0,
		nullptr,
		&m_Data
	);
	if (result != MA_SUCCESS)
		spdlog::error("Failed to initialise sound source - sound engine error [{}]", (int)result);
	else
		ma_sound_get_length_in_seconds(&m_Data, &m_Length);

	// Update values //
	SetPitch(m_Pitch);
	SetSpatialization(m_Spatialization);

	if(m_Mixer) // If mixer is not default output
		SetMixer(m_Mixer);
}

ResourceID AudioSource::GetMixer() { return m_Mixer; }
void AudioSource::SetMixer(ResourceID mixer)
{
	m_Mixer = mixer;

	if(m_Sound == InvalidResourceID)
		return; // No sound loaded

	ma_node* output = nullptr;

	// Check for valid mixer
	if(mixer == InvalidResourceID || !Resource::IsValidType<AudioMixer>(mixer))
		output = ma_engine_get_endpoint(AudioSystem::GetEngine());
	else
		output = (ma_node*)Resource::Get<AudioMixer>(mixer)->GetHandle();

	ma_node_attach_output_bus(&m_Data, 0, output, 0);
}

ma_sound* AudioSource::GetHandle() { return &m_Data; }

void AudioSource::UpdateManagedState()
{
	if (!AudioSourceUpdateManagedState) return;

	MonoException* exception = nullptr;
	AudioSourceUpdateManagedState(ManagedData.GetInstance(), (unsigned int)m_State, &exception);
}

#pragma region Scripting Internal Calls
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(AudioSource, GetSound, uint64_t, (void* instance))
{ return ((AudioSource*)instance)->GetSound(); }

ADD_MANAGED_METHOD(AudioSource, SetSound, void, (void* instance, uint64_t value))
{ ((AudioSource*)instance)->SetSound(value); }

ADD_MANAGED_METHOD(AudioSource, Play, void, (void* instance))
{ ((AudioSource*)instance)->Play(); }

ADD_MANAGED_METHOD(AudioSource, Pause, void, (void* instance))
{ ((AudioSource*)instance)->Pause(); }

ADD_MANAGED_METHOD(AudioSource, Stop, void, (void* instance))
{ ((AudioSource*)instance)->Stop(); }

ADD_MANAGED_METHOD(AudioSource, Seek, void, (void* instance, float seconds))
{ ((AudioSource*)instance)->Seek(seconds); }

ADD_MANAGED_METHOD(AudioSource, IsPlaying, bool, (void* instance))
{ return ((AudioSource*)instance)->IsPlaying(); }

ADD_MANAGED_METHOD(AudioSource, GetState, unsigned int, (void* instance))
{ return (unsigned int)((AudioSource*)instance)->GetState(); }

ADD_MANAGED_METHOD(AudioSource, GetPlayTime, float, (void* instance))
{ return ((AudioSource*)instance)->GetPlayTime(); }

ADD_MANAGED_METHOD(AudioSource, GetLength, float, (void* instance))
{ return ((AudioSource*)instance)->GetLength(); }

ADD_MANAGED_METHOD(AudioSource, GetLooping, bool, (void* instance))
{ return ((AudioSource*)instance)->IsLooping(); }

ADD_MANAGED_METHOD(AudioSource, SetLooping, void, (void* instance, bool value))
{ ((AudioSource*)instance)->SetLooping(value); }

ADD_MANAGED_METHOD(AudioSource, GetSpatialization, bool, (void* instance))
{ return ((AudioSource*)instance)->GetSpatialization(); }

ADD_MANAGED_METHOD(AudioSource, SetSpatialization, void, (void* instance, bool value))
{ ((AudioSource*)instance)->SetSpatialization(value); }

ADD_MANAGED_METHOD(AudioSource, GetMixer, uint64_t, (void* instance))
{ return ((AudioSource*)instance)->GetMixer(); }

ADD_MANAGED_METHOD(AudioSource, SetMixer, void, (void* instance, uint64_t value))
{ ((AudioSource*)instance)->SetMixer(value); }

ADD_MANAGED_METHOD(AudioSource, GetPanning, float, (void* instance))
{ return ((AudioSource*)instance)->GetPanning(); }

ADD_MANAGED_METHOD(AudioSource, SetPanning, void, (void* instance, float value))
{ ((AudioSource*)instance)->SetPanning(value); }

ADD_MANAGED_METHOD(AudioSource, GetVolume, float, (void* instance))
{ return ((AudioSource*)instance)->GetVolume(); }

ADD_MANAGED_METHOD(AudioSource, SetVolume, void, (void* instance, float value))
{ ((AudioSource*)instance)->SetVolume(value); }

ADD_MANAGED_METHOD(AudioSource, GetPitch, float, (void* instance))
{ return ((AudioSource*)instance)->GetPitch(); }

ADD_MANAGED_METHOD(AudioSource, SetPitch, void, (void* instance, float value))
{ ((AudioSource*)instance)->SetPitch(value); }

#pragma endregion
