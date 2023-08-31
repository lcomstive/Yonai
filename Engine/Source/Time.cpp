#include <spdlog/spdlog.h>
#include <Yonai/Time.hpp>

using namespace Yonai;
using namespace std::chrono;

double Time::s_FPS = 0;
int Time::s_FrameCount = 0;
double Time::s_DeltaTime = 0;
double Time::s_TimePassed = 0;
double Time::s_FrameTimeAccumulator = 0.0;
time_point<high_resolution_clock> Time::s_FrameStartTime = high_resolution_clock::now();

void Time::OnFrameEnd()
{
	auto frameEndTime = high_resolution_clock::now();

	// Nanoseconds -> Microseconds -> Milliseconds -> Seconds = Nanoseconds / 1,000,000,000
	s_DeltaTime = (frameEndTime - s_FrameStartTime).count() * 0.000000001;
	s_FrameStartTime = frameEndTime;

	// Add values
	s_FrameCount++;
	s_FrameTimeAccumulator += s_DeltaTime;
	s_TimePassed += s_DeltaTime;

	// Check if one second has passed
	if (s_FrameTimeAccumulator >= 1.0)
	{
		s_FPS = float(s_FrameCount);

		// Reset stats
		s_FrameCount = 0;
		s_FrameTimeAccumulator = 0;
	}
}

double Time::FPS() { return s_FPS; }
double Time::DeltaTime() { return s_DeltaTime; }
double Time::SinceLaunch() { return s_TimePassed; }