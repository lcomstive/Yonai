#include <EditorApp.hpp>
#include <SpashScreen.hpp>
#include <AquaEngine/API.hpp>

int main(int argc, char** argv)
{
#if !defined(BUILD_SHARED_LIBS) // Splashscreen currently does not work across .dll boundaries
	AquaEditor::SplashScreenApp splashScreen;
	splashScreen.ProcessArgs(argc, argv);
	splashScreen.Run();

	// Check if error occurred, or user requested closing
	if(splashScreen.LaunchDisrupted())
		return -1;
#endif
	
	// Launch editor
	AquaEditor::EditorApp app;
	app.ProcessArgs(argc, argv);
	
	app.Run(); // Application loop

	return 0;
}

// Check for Windows compilation, this will be with the "/ENTRY:WinMainCRTStartup" flag
#if defined(NDEBUG) && defined(AQUA_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>

int WINAPI WinMain(
	_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE previous,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
	)
{
	return main(__argc, __argv);
}
#endif