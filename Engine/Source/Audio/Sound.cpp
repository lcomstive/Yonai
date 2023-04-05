#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Audio/Sound.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;

Sound::Sound() : m_Sound() { }
Sound::Sound(string& filepath) : m_Sound() { Import(filepath); }

Sound::~Sound() { ma_sound_uninit(&m_Sound); }

float Sound::GetLength()
{
	float output;
	ma_sound_get_length_in_seconds(&m_Sound, &output);
	return output;
}

void Sound::Import(const char* filepath) { Import(string(filepath)); }
void Sound::Import(string filepath)
{
	if (!m_FilePath.empty())
		// Unload previously loaded sound
		ma_sound_uninit(&m_Sound);

	m_FilePath = filepath;

	// Check if there is a sound to load
	if (m_FilePath.empty())
		return;

	string assetPath = VFS::GetAbsolutePath(m_FilePath, true);

	ma_result result = ma_sound_init_from_file(&AudioSystem::s_Engine, assetPath.c_str(), c_Flags, nullptr, nullptr, &m_Sound);
	if (result != MA_SUCCESS)
	{
		spdlog::warn("Failed to create sound for '{}' [{}]", m_FilePath.c_str(), (int)result);
		return;
	}
}

#pragma region Scripting Bindings
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Sound, Load, void, (MonoString* path, unsigned long long* outResourceID, void** outHandle))
{
	char* resourcePath = mono_string_to_utf8(path);
	*outResourceID = Resource::Load<Sound>(resourcePath);
	*outHandle = Resource::Get<Sound>(*outResourceID);
	mono_free(resourcePath);
}

ADD_MANAGED_METHOD(Sound, Import, void, (void* handle, MonoString* path))
{
	char* filepath = mono_string_to_utf8(path);
	((Sound*)handle)->Import(filepath);
	mono_free(filepath);
}
#pragma endregion
