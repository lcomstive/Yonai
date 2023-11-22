#include <GLFW/glfw3.h>

#include <Yonai/Window.hpp>
#include <Yonai/Resource.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace Yonai;

ADD_MANAGED_METHOD(EditorWindow, InitContext, void, (), YonaiEditor)
{ Window::InitContext(); }

ADD_MANAGED_METHOD(EditorWindow, DestroyContext, void, (), YonaiEditor)
{ Window::DestroyContext(); }

ADD_MANAGED_METHOD(EditorWindow, ContextIsInitialised, bool, (), YonaiEditor)
{ return Window::ContextIsInitialised(); }

ADD_MANAGED_METHOD(EditorWindow, Create, void, (), YonaiEditor)
{ Window::Create(); }

ADD_MANAGED_METHOD(EditorWindow, Show, void, (bool show), YonaiEditor)
{ Window::Show(show); }

ADD_MANAGED_METHOD(EditorWindow, Destroy, void, (), YonaiEditor)
{ Window::Destroy(); }

ADD_MANAGED_METHOD(EditorWindow, _CreationHintBool, void, (int hint, bool value), YonaiEditor)
{
#ifdef YONAI_PLATFORM_DESKTOP
	if (!Window::GetNativeHandle())
		glfwWindowHint(hint, value);
	else
		glfwSetWindowAttrib(Window::GetNativeHandle(), hint, value);
#endif
}

ADD_MANAGED_METHOD(EditorWindow, _CreationHintInt, void, (int hint, int value), YonaiEditor)
{
#ifdef YONAI_PLATFORM_DESKTOP
	if (!Window::GetNativeHandle())
		glfwWindowHint(hint, value);
	else
		glfwSetWindowAttrib(Window::GetNativeHandle(), hint, value);
#endif
}

ADD_MANAGED_METHOD(EditorWindow, ResetHints, void, (), YonaiEditor)
{
#ifdef YONAI_PLATFORM_DESKTOP
	glfwDefaultWindowHints();
#endif
}