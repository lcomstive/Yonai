#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Audio/Sound.hpp>
#include <AquaEngine/Components/SoundSource.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Components;

void SoundSource::Play()
{
	// If already playing, stop and reset
	if (IsPlaying())
		Stop();

	// Check if paused, resume if so
	if (m_State == SoundState::Paused)
	{
		Resume();
		return;
	}

	// Ensure clip is valid
	if (SoundClip == InvalidResourceID)
	{
		spdlog::warn("Tried playing sound source [{}.{}] but no sound was set", Entity.GetWorld()->ID(), Entity.ID());
		return;
	}

	Sound* sound = Resource::Get<Sound>(SoundClip);
	m_Data = sound->m_Source;
	m_State = SoundState::Playing;

	// Get sample rate from sound
	m_SampleRate = sound->GetSampleRate();

	// Reset frame counts
	ma_decoder_get_length_in_pcm_frames(&m_Data, &m_TotalFrames);

	// Seek to start
	ma_decoder_seek_to_pcm_frame(&m_Data, 0);

	// Setup node
	ma_data_source_node_config nodeConfig = ma_data_source_node_config_init(&m_Data);
	ma_data_source_node_init(&AudioSystem::s_NodeGraph, &nodeConfig, nullptr, &m_Node);
	ma_node_attach_output_bus(&m_Node, 0, ma_node_graph_get_endpoint(&AudioSystem::s_NodeGraph), 0);

	// Update node volume
	SetVolume(m_Volume);
}

void SoundSource::Pause()
{
	if (m_State != SoundState::Playing)
		return;

	m_State = SoundState::Paused;
	ma_node_set_state(&m_Node, ma_node_state_stopped);
}

void SoundSource::Resume()
{
	if (m_State != SoundState::Paused)
		return;

	m_State = SoundState::Playing;
	ma_node_set_state(&m_Node, ma_node_state_started);
}

void SoundSource::Stop()
{
	// Set state
	m_State = SoundState::Stopped;
	
	// Reset frame counts
	m_TotalFrames = 0;

	// Cleanup node
	ma_data_source_node_uninit(&m_Node, nullptr);
}

float SoundSource::GetVolume() { return m_Volume; }
SoundState SoundSource::GetState() { return m_State; }
bool SoundSource::IsPlaying() { return m_State == SoundState::Playing; }
float SoundSource::GetLength()
{
	return m_State == SoundState::Stopped || m_SampleRate == 0 ?
		0.0f : m_TotalFrames / m_SampleRate;
}

float SoundSource::GetPlayTime()
{
	if (m_State == SoundState::Stopped || m_SampleRate == 0)
		return 0.0f;

	ma_uint64 time = ma_node_get_time(&m_Node);
	return time / m_SampleRate;
}

void SoundSource::Seek(float seconds)
{
	if (m_State == SoundState::Stopped)
		return;

	// Ensure value is within bounds
	seconds = std::clamp(seconds, 0.0f, GetLength());

	ma_uint64 pcmTime = (ma_uint64)std::floor(seconds * m_SampleRate);

	ma_data_source_seek_to_pcm_frame(&m_Data, pcmTime);
	ma_node_set_time(&m_Node, pcmTime);
}

void SoundSource::SetVolume(float volume)
{
	// Ensure volume is at least 0
	m_Volume = (std::max)(volume, 0.0f);

	if(m_State != SoundState::Stopped)
		ma_node_set_output_bus_volume(&m_Node, 0, m_Volume);
}

#pragma region Scripting Internal Calls
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_GET_SET(SoundSource, SoundClip, unsigned int)

ADD_MANAGED_METHOD(SoundSource, Play, void, (void* instance))
{ ((SoundSource*)instance)->Play(); }

ADD_MANAGED_METHOD(SoundSource, Pause, void, (void* instance))
{ ((SoundSource*)instance)->Pause(); }

ADD_MANAGED_METHOD(SoundSource, Stop, void, (void* instance))
{ ((SoundSource*)instance)->Stop(); }

ADD_MANAGED_METHOD(SoundSource, IsPlaying, bool, (void* instance))
{ return ((SoundSource*)instance)->IsPlaying(); }

ADD_MANAGED_METHOD(SoundSource, GetState, unsigned int, (void* instance))
{ return (unsigned int)((SoundSource*)instance)->GetState(); }
#pragma endregion
