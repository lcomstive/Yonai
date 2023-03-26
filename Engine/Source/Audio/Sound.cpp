#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Audio/Sound.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;

Sound::Sound(const string& filepath)
{
	string assetPath = VFS::GetAbsolutePath(filepath, true);

	ma_result result = ma_sound_init_from_file(&AudioSystem::s_Engine, assetPath.c_str(), c_Flags, nullptr, nullptr, &m_Sound);
	if (result != MA_SUCCESS)
	{
		spdlog::warn("Failed to create sound for '{}' [{}]", filepath.c_str(), (int)result);
		return;
	}
}

Sound::~Sound() { ma_sound_uninit(&m_Sound); }

float Sound::GetLength()
{
	float output;
	ma_sound_get_length_in_seconds(&m_Sound, &output);
	return output;
}

#pragma region Scripting Bindings
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Sound, Load, void, (MonoString* path, MonoString* audioFilePath, unsigned int* outResourceID, void** outHandle))
{
	*outResourceID = Resource::Load<Sound>(mono_string_to_utf8(path), mono_string_to_utf8(audioFilePath));
	*outHandle = Resource::Get<Sound>(*outResourceID);
}
#pragma endregion