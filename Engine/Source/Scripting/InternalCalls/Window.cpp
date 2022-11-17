#include <glm/glm.hpp>
#include <mono/jit/jit.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>

using namespace AquaEngine;

void SetResolution(glm::vec2* value) { Window::SetResolution(*value); }
void GetResolution(glm::vec2* outValue) { *outValue = Window::GetResolution(); }

void SetFullscreenMode(int state) { Window::SetFullscreen((FullscreenMode)state); }
int GetFullscreenMode() { return (int)Window::GetFullscreen(); }

int GetVideoModes(MonoArray** outBits, MonoArray** outWidths, MonoArray** outHeights, MonoArray** outRefreshRates)
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

#define ADD_WINDOW_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.Window::_aqua_internal_"#name, (const void*)name);
#define ADD_SCREEN_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.Screen::_aqua_internal_"#name, (const void*)name);

void AquaEngine::Scripting::Assembly::AddWindowInternalCalls()
{
	ADD_WINDOW_INTERNAL_CALL(GetResolution)
	ADD_WINDOW_INTERNAL_CALL(SetResolution)

	ADD_SCREEN_INTERNAL_CALL(GetVideoModes)

	ADD_WINDOW_INTERNAL_CALL(GetFullscreenMode)
	ADD_WINDOW_INTERNAL_CALL(SetFullscreenMode)
}
