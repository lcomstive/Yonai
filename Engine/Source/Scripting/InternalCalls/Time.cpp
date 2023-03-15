#include <mono/jit/jit.h>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;

ADD_MANAGED_METHOD(Time, FPS, float)
{ return (float)Time::FPS(); }

ADD_MANAGED_METHOD(Time, DeltaTime, float)
{ return (float)Time::DeltaTime(); }

ADD_MANAGED_METHOD(Time, TimeSinceLaunch, float)
{ return (float)Time::SinceLaunch(); }
