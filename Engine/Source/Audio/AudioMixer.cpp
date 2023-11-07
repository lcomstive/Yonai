#include <Yonai/Audio/AudioMixer.hpp>
#include <Yonai/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Systems;

AudioMixer::AudioMixer(string name, ResourceID parent) : Name(name), m_Volume(1.0f), m_Parent(parent)
{
	ma_sound_group_config config = ma_sound_group_config_init();
	ma_sound_group_init(AudioSystem::GetEngine(), 0, nullptr, &m_Group);

	if(m_Parent != InvalidResourceID)
		SetParent(m_Parent);
}

AudioMixer::~AudioMixer() { ma_sound_group_uninit(&m_Group); }

ma_sound_group* AudioMixer::GetHandle() { return &m_Group; }

float AudioMixer::GetVolume() { return m_Volume; }
void AudioMixer::SetVolume(float value)
{
	m_Volume = (std::max)(value, 0.0f);
	ma_sound_group_set_volume(&m_Group, m_Volume);
}

void AudioMixer::SetParent(ResourceID parent)
{
	m_Parent = parent;
	ma_node* output = nullptr;
	if(m_Parent == InvalidResourceID || !Resource::IsValidType<AudioMixer>(m_Parent)) // Set to output to the engine directly (AKA master)
		output = ma_engine_get_endpoint(AudioSystem::GetEngine());
	else
		output = &(Resource::Get<AudioMixer>(m_Parent)->m_Group);

	ma_node_attach_output_bus(&m_Group, 0, output, 0);
}

ResourceID AudioMixer::GetParent() { return m_Parent; }

#pragma region Scripting Bindings
#include <Yonai/Resource.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(AudioMixer, Load, void, (MonoString* pathRaw, uint64_t* outResourceID, void** outHandle))
{
	char* path = mono_string_to_utf8(pathRaw);

	if (*outResourceID == InvalidResourceID)
		*outResourceID = ResourceID(); // If no resource ID, generate one
	Resource::Load<AudioMixer>(*outResourceID, path);

	*outHandle = Resource::Get<AudioMixer>(*outResourceID);
	mono_free(path);
}

ADD_MANAGED_METHOD(AudioMixer, GetName, MonoString*, (void* handle))
{ return mono_string_new(mono_domain_get(), ((AudioMixer*)handle)->Name.c_str()); }

ADD_MANAGED_METHOD(AudioMixer, SetName, void, (void* handle, MonoString* value))
{ ((AudioMixer*)handle)->Name = mono_string_to_utf8(value); }

ADD_MANAGED_METHOD(AudioMixer, GetVolume, float, (void* handle))
{ return ((AudioMixer*)handle)->GetVolume(); }

ADD_MANAGED_METHOD(AudioMixer, SetVolume, void, (void* handle, float value))
{ ((AudioMixer*)handle)->SetVolume(value); }

ADD_MANAGED_METHOD(AudioMixer, GetParent, uint64_t, (void* handle))
{ return ((AudioMixer*)handle)->GetParent(); }

ADD_MANAGED_METHOD(AudioMixer, SetParent, void, (void* handle, uint64_t value))
{ return ((AudioMixer*)handle)->SetParent(value); }
#pragma endregion