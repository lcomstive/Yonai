#include <stdlib.h> 
#include <iostream>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Systems/System.hpp>

using namespace std;
using namespace AquaEngine;

class EditorApp : public WindowedApplication
{
	void OnUpdate() override
	{
		Window::SetTitle("Aqua Editor [" + to_string(int(Time::FPS())) + " FPS]");
	}
};

#if defined(NDEBUG) && defined(AQUA_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
	EditorApp app;

	Window::SetTitle("Aqua Editor");

	app.Run();

	return 0;
}
