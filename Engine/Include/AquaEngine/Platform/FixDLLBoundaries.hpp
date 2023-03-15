#pragma once
#include <AquaEngine/API.hpp>

#if defined(BUILD_SHARED_LIBS)
	#include <imgui.h>
	#include <AquaEngine/Window.hpp>
	#include <AquaEngine/SystemManager.hpp>
	#include <AquaEngine/Systems/Global/ImGUISystem.hpp>

	#if defined(AQUA_PLATFORM_DESKTOP)
		#define FIX_DLL_BOUNDARIES() \
			ImGui::SetCurrentContext(SystemManager::Global()->Get<Systems::ImGUISystem>()->GetContext()); \
			\
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
