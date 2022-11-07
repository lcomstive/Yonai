#include <mono/jit/jit.h>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>

using namespace AquaEngine;

float GetFPS() { return (float)Time::FPS(); }
float GetDeltaTime() { return (float)Time::DeltaTime(); }
float TimeSinceLaunch() { return (float)Time::SinceLaunch(); }

void AquaEngine::Scripting::Assembly::AddTimeInternalCalls()
{
	mono_add_internal_call("AquaEngine.Time::_aqua_internal_GetFPS", (const void*)GetFPS);
	mono_add_internal_call("AquaEngine.Time::_aqua_internal_GetDeltaTime", (const void*)GetDeltaTime);
	mono_add_internal_call("AquaEngine.Time::_aqua_internal_TimeSinceLaunch", (const void*)TimeSinceLaunch);
}