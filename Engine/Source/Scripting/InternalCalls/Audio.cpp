#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace AquaEngine::Systems;

ADD_MANAGED_METHOD(Audio, GetDeviceCount, unsigned int)
{ return AudioSystem::GetDeviceCount(); }

ADD_MANAGED_METHOD(Audio, GetDefaultDevice, unsigned int)
{ return AudioSystem::GetDefaultDevice(); }

ADD_MANAGED_METHOD(Audio, GetDeviceName, MonoString*, (unsigned int index))
{ return mono_string_new(mono_domain_get(), AudioSystem::GetDeviceName(index)); }

ADD_MANAGED_METHOD(Audio, SetOutputDevice, void, (unsigned int index))
{ AudioSystem::SetOutputDevice(index); }

ADD_MANAGED_METHOD(Audio, GetOutputDevice, unsigned int)
{ return AudioSystem::GetOutputDevice(); }