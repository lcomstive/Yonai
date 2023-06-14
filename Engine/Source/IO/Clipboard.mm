#include <AquaEngine/API.hpp>
#if defined(AQUA_PLATFORM_APPLE)
#include <AquaEngine/IO/Clipboard.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine::IO;

#include <Foundation/Foundation.h>

#if defined(AQUA_PLATFORM_MAC) // Desktop
#include <AppKit/NSPasteboard.h>
#elif defined(AQUA_PLATFORM_iOS) // Mobile
// TOOD: Test this
#endif

void Clipboard::SetText(char* text) { SetText(string(text)); }

void Clipboard::SetText(string text)
{
	NSString* buffer = text.empty() ? @"" :
						[NSString stringWithCString : text.c_str()
							encoding : [NSString defaultCStringEncoding] ];
}

string Clipboard::GetText()
{
#if defined(AQUA_PLATFORM_MAC) // Desktop
	NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
	return string([[pasteboard stringForType:NSPasteboardTypeString] UTF8String]);
#elif defined(AQUA_PLATFORM_iOS) // Mobile
	UIPasteboard* pasteboard = [UIPasteboard generalPasteboard];
	return string([pasteboard.string UTF8String]);
#endif
}
#endif