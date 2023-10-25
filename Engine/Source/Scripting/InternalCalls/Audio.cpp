#include <Yonai/Scripting/InternalCalls.hpp>
#include <Yonai/Systems/Global/AudioSystem.hpp>

using namespace std;
using namespace Yonai::Systems;

ADD_MANAGED_METHOD(AudioData, GetDeviceCount, unsigned int)
{ return AudioSystem::GetDeviceCount(); }

ADD_MANAGED_METHOD(AudioData, GetDefaultDevice, unsigned int)
{ return AudioSystem::GetDefaultDevice(); }

ADD_MANAGED_METHOD(AudioData, GetDeviceName, MonoString*, (unsigned int index))
{ return mono_string_new(mono_domain_get(), AudioSystem::GetDeviceName(index)); }

ADD_MANAGED_METHOD(AudioData, SetOutputDevice, void, (unsigned int index))
{ AudioSystem::SetOutputDevice(index); }

ADD_MANAGED_METHOD(AudioData, GetOutputDevice, unsigned int)
{ return AudioSystem::GetOutputDevice(); }