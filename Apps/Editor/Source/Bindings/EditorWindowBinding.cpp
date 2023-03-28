#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <AquaEngine/Window.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Graphics;

ADD_MANAGED_METHOD(EditorWindow, InitContext, void, (), AquaEditor)
{ Window::InitContext(); }

ADD_MANAGED_METHOD(EditorWindow, DestroyContext, void, (), AquaEditor)
{ Window::DestroyContext(); }

ADD_MANAGED_METHOD(EditorWindow, Create, void, (), AquaEditor)
{
	Window::Create();

#ifdef AQUA_PLATFORM_DESKTOP
	glfwMakeContextCurrent(Window::GetNativeHandle());
	gladLoadGL(glfwGetProcAddress);
#endif
}

ADD_MANAGED_METHOD(EditorWindow, Destroy, void, (), AquaEditor)
{ Window::Destroy(); }

ADD_MANAGED_METHOD(EditorWindow, _CreationHintBool, void, (int hint, bool value), AquaEditor)
{
#ifdef AQUA_PLATFORM_DESKTOP
	if (!Window::GetNativeHandle())
		glfwWindowHint(hint, value);
	else
		glfwSetWindowAttrib(Window::GetNativeHandle(), hint, value);
#endif
}

ADD_MANAGED_METHOD(EditorWindow, _CreationHintInt, void, (int hint, int value), AquaEditor)
{
#ifdef AQUA_PLATFORM_DESKTOP
	if (!Window::GetNativeHandle())
		glfwWindowHint(hint, value);
	else
		glfwSetWindowAttrib(Window::GetNativeHandle(), hint, value);
#endif
}

ADD_MANAGED_METHOD(EditorWindow, ResetHints, void, (), AquaEditor)
{
#ifdef AQUA_PLATFORM_DESKTOP
	glfwDefaultWindowHints();
#endif
}