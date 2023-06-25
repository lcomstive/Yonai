#pragma once
#include <AquaEngine/API.hpp>

#if defined(BUILD_SHARED_LIBS)
	#include <imgui.h>
	#include <AquaEngine/Window.hpp>
	#include <AquaEngine/SystemManager.hpp>

	#if defined(AQUA_PLATFORM_DESKTOP)
		#define FIX_DLL_BOUNDARIES() \
			glfwMakeContextCurrent(Window::GetNativeHandle()); \
			gladLoadGL(glfwGetProcAddress);
	#else
		#define FIX_DLL_BOUNDARIES() \
				ImGuiContext* context = SystemManager::Global()->Get<Systems::ImGUISystem>()->GetContext(); \
				ImGui::SetCurrentContext(context);
	#endif
#else
#define FIX_DLL_BOUNDARIES()
#endif
