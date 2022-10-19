#include <mono/jit/jit.h>
#include <AquaEngine/Time.hpp>

using namespace AquaEngine;

namespace InternalCalls
{
	float GetFPS() { return (float)Time::FPS(); }
	float GetDeltaTime() { return (float)Time::DeltaTime(); }
	float TimeSinceLaunch() { return (float)Time::SinceLaunch(); }

	void AddTimeInternalCalls()
	{
		mono_add_internal_call("AquaEngine.Time::_aqua_internal_GetFPS", (const void*)InternalCalls::GetFPS);
		mono_add_internal_call("AquaEngine.Time::_aqua_internal_GetDeltaTime", (const void*)InternalCalls::GetDeltaTime);
		mono_add_internal_call("AquaEngine.Time::_aqua_internal_TimeSinceLaunch", (const void*)InternalCalls::TimeSinceLaunch);
	}
}