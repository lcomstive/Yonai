#include <AquaEngine/Audio/SoundMixer.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

SoundMixer::SoundMixer(string name, ResourceID parent) : Name(name), m_Volume(1.0f)
{
	ma_sound_group_config config = ma_sound_group_config_init();
	ma_sound_group_init(AudioSystem::GetEngine(), 0, nullptr, &m_Group);

	if(parent != InvalidResourceID)
		SetParent(parent);
}

SoundMixer::~SoundMixer() { ma_sound_group_uninit(&m_Group); }

ma_sound_group* SoundMixer::GetHandle() { return &m_Group; }

float SoundMixer::GetVolume() { return m_Volume; }
void SoundMixer::SetVolume(float value)
{
	m_Volume = std::max(value, 0.0f);
	ma_sound_group_set_volume(&m_Group, m_Volume);
}

void SoundMixer::SetParent(ResourceID parent)
{
	ma_node* output = nullptr;
	if(parent == InvalidResourceID || !Resource::IsValidType<SoundMixer>(parent)) // Set to output to the engine directly (AKA master)
		output = ma_engine_get_endpoint(AudioSystem::GetEngine());
	else
		output = &(Resource::Get<SoundMixer>(parent)->m_Group);

	ma_node_attach_output_bus(&m_Group, 0, output, 0);
}

#pragma region Scripting Bindings
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(SoundMixer, Load0, void, (MonoString* path, MonoString* mixerName, unsigned int* outResourceID, void** outHandle))
{
	*outResourceID = Resource::Load<SoundMixer>(mono_string_to_utf8(path), mono_string_to_utf8(mixerName));
	*outHandle = Resource::Get<SoundMixer>(*outResourceID);
}

ADD_MANAGED_METHOD(SoundMixer, Load1, void, (MonoString* path, MonoString* mixerName, unsigned int parent, unsigned int* outResourceID, void** outHandle))
{
	*outResourceID = Resource::Load<SoundMixer>(mono_string_to_utf8(path), mono_string_to_utf8(mixerName), parent);
	*outHandle = Resource::Get<SoundMixer>(*outResourceID);
}

ADD_MANAGED_METHOD(SoundMixer, GetName, MonoString*, (void* handle))
{ return mono_string_new(mono_domain_get(), ((SoundMixer*)handle)->Name.c_str()); }

ADD_MANAGED_METHOD(SoundMixer, SetName, void, (void* handle, MonoString* value))
{ ((SoundMixer*)handle)->Name = mono_string_to_utf8(value); }

ADD_MANAGED_METHOD(SoundMixer, GetVolume, float, (void* handle))
{ return ((SoundMixer*)handle)->GetVolume(); }

ADD_MANAGED_METHOD(SoundMixer, SetVolume, void, (void* handle, float value))
{ ((SoundMixer*)handle)->SetVolume(value); }
#pragma endregion