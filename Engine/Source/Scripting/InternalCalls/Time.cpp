#include <mono/jit/jit.h>
#include <Yonai/Time.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace Yonai;

ADD_MANAGED_METHOD(Time, FPS, float)
{ return (float)Time::FPS(); }

ADD_MANAGED_METHOD(Time, DeltaTime, float)
{ return (float)Time::DeltaTime(); }

ADD_MANAGED_METHOD(Time, TimeSinceLaunch, float)
{ return (float)Time::SinceLaunch(); }
