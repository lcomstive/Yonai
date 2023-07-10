#include <BaseGame.hpp>

int main(int argc, char** argv)
{
	BaseGame(argc, argv).Run();
	return 0;
}

// Check for Windows compilation, this will be with the "/ENTRY:WinMainCRTStartup" flag
#if defined(NDEBUG) && defined(YONAI_PLATFORM_WINDOWS)
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