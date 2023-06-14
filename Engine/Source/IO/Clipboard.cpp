#include <AquaEngine/API.hpp>
#include <AquaEngine/IO/Clipboard.hpp>

using namespace std;
using namespace AquaEngine::IO;

#if !defined(AQUA_PLATFORM_APPLE)
#if defined(AQUA_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(AQUA_PLATFORM_APPLE)
// Functionality placed in Clipboard.mm
#elif defined(AQUA_PLATFORM_ANDROID)

#elif defined(AQUA_PLATFORM_LINUX)
#include <X11/Xlib.h>
#endif

void Clipboard::SetText(char* text) { SetText(string(text)); }

void Clipboard::SetText(string text)
{
#if defined(AQUA_PLATFORM_WINDOWS)
	HGLOBAL hgl = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	memcpy(GlobalLock(hgl), text.c_str(), text.size() + 1);
	GlobalUnlock(hgl);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hgl);
	CloseClipboard();
#endif
}

string Clipboard::GetText()
{
#if defined(AQUA_PLATFORM_WINDOWS)
	OpenClipboard(NULL);
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr)
		return string();

	char* textRaw = (char*)GlobalLock(hData);
	string text(textRaw); // Copy data for return

	// Release lock
	GlobalUnlock(textRaw);
	CloseClipboard();

	return text;
#else
	return string();
#endif
}
#endif

#pragma region Internal Calls
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Clipboard, SetText, void, (MonoString* value), AquaEngine.IO)
{
	char* contents = mono_string_to_utf8(value);
	Clipboard::SetText(contents);
	mono_free(contents);
}

ADD_MANAGED_METHOD(Clipboard, GetText, MonoString*, (), AquaEngine.IO)
{
	string contents = Clipboard::GetText();
	return mono_string_new(mono_domain_get(), contents.c_str());
}
#pragma endregion