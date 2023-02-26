#include <vector>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine;

ADD_MANAGED_METHOD(Window, SetResolution, void, (glm::ivec2* value))
{ Window::SetResolution(*value); }
ADD_MANAGED_METHOD(Window, GetResolution, void, (glm::ivec2* outValue))
{ *outValue = Window::GetResolution(); }

ADD_MANAGED_METHOD(Window, SetFullscreenMode, void, (int state))
{ Window::SetFullscreen((FullscreenMode)state); }
ADD_MANAGED_METHOD(Window, GetFullscreenMode, int)
{ return (int)Window::GetFullscreen(); }

ADD_MANAGED_METHOD(Screen, GetVideoModes, unsigned int, (MonoArray** outWidths, MonoArray** outHeights, MonoArray** outRefreshRates))
{
	vector<VideoMode> modes = Window::GetVideoModes();

	*outWidths		 = mono_array_new(mono_domain_get(), mono_get_int32_class(), modes.size());
	*outHeights		 = mono_array_new(mono_domain_get(), mono_get_int32_class(), modes.size());
	*outRefreshRates = mono_array_new(mono_domain_get(), mono_get_single_class(), modes.size());

#ifdef AQUA_PLATFORM_DESKTOP
	for (size_t i = 0; i < modes.size(); i++)
	{
		mono_array_set(*outWidths,		 int,	i, modes[i].Resolution.x);
		mono_array_set(*outHeights,		 int,	i, modes[i].Resolution.y);
		mono_array_set(*outRefreshRates, float, i, modes[i].RefreshRate);
	}
#endif

	return (unsigned int)modes.size();
}

ADD_MANAGED_METHOD(Screen, GetVideoMode, void, (glm::ivec2* outResolution, float* outRefreshRate))
{
	VideoMode mode = Window::GetCurrentVideoMode();
	*outResolution = mode.Resolution;
	*outRefreshRate = mode.RefreshRate;
}

ADD_MANAGED_METHOD(Screen, SetVideoMode, void, (glm::ivec2* resolution, float refreshRate))
{ Window::SetVideoMode(VideoMode { *resolution, refreshRate }); }

ADD_MANAGED_METHOD(Window, CenterOnDisplay)
{ Window::CenterOnDisplay(); }