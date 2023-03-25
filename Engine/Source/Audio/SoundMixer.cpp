#include <AquaEngine/Audio/SoundMixer.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

SoundMixer::SoundMixer(string name, SoundMixer* parent) : Name(name), m_Volume(1.0f)
{
	ma_sound_group_config config = ma_sound_group_config_init();
	ma_sound_group_init(AudioSystem::GetEngine(), 0, parent ? & parent->m_Group : nullptr, &m_Group);
}

SoundMixer::~SoundMixer() { ma_sound_group_uninit(&m_Group); }

ma_sound_group* SoundMixer::GetHandle() { return &m_Group; }

float SoundMixer::GetVolume() { return m_Volume; }
void SoundMixer::SetVolume(float value)
{
	m_Volume = std::max(value, 0.0f);
	ma_sound_group_set_volume(&m_Group, m_Volume);
}

void SoundMixer::SetParent(SoundMixer* other)
{
	ma_node* output = nullptr;
	if(!other)
		output = ma_engine_get_endpoint(AudioSystem::GetEngine());
	else
		output = &other->m_Group;

	ma_node_attach_output_bus(&m_Group, 0, output, 0);
}