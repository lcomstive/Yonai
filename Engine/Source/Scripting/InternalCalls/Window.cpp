#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;

ADD_MANAGED_METHOD(Window, SetResolution, void, (glm::vec2* value))
{ Window::SetResolution(*value); }
ADD_MANAGED_METHOD(Window, GetResolution, void, (glm::vec2* outValue))
{ *outValue = Window::GetResolution(); }

ADD_MANAGED_METHOD(Window, SetFullscreenMode, void, (int state))
{ Window::SetFullscreen((FullscreenMode)state); }
ADD_MANAGED_METHOD(Window, GetFullscreenMode, int)
{ return (int)Window::GetFullscreen(); }

ADD_MANAGED_METHOD(Screen, GetVideoModes, int, (MonoArray** outBits, MonoArray** outWidths, MonoArray** outHeights, MonoArray** outRefreshRates))
{
	int modesCount = 0;

#ifdef AQUA_PLATFORM_DESKTOP
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* modes = glfwGetVideoModes(monitor, &modesCount);
#endif

	*outBits		 = mono_array_new(mono_domain_get(), mono_get_int32_class(), modesCount);
	*outWidths		 = mono_array_new(mono_domain_get(), mono_get_int32_class(), modesCount);
	*outHeights		 = mono_array_new(mono_domain_get(), mono_get_int32_class(), modesCount);
	*outRefreshRates = mono_array_new(mono_domain_get(), mono_get_int32_class(), modesCount);

#ifdef AQUA_PLATFORM_DESKTOP
	for (int i = 0; i < modesCount; i++)
	{
		GLFWvidmode mode = modes[i];
		mono_array_set(*outBits,		 int, i, mode.redBits);
		mono_array_set(*outWidths,		 int, i, mode.width);
		mono_array_set(*outHeights,		 int, i, mode.height);
		mono_array_set(*outRefreshRates, int, i, mode.refreshRate);
	}
#endif

	return modesCount;
}
