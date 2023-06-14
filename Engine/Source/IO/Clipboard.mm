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
#import <UIKit/UIPasteboard.h>
#endif

void Clipboard::SetText(char* text) { SetText(string(text)); }

void Clipboard::SetText(string text)
{
	NSString* buffer = [NSString stringWithUTF8String: text.c_str()];

#if defined(AQUA_PLATFORM_MAC) // Desktop
	[[NSPasteboard generalPasteboard] declareTypes: [NSArray arrayWithObject: NSPasteboardTypeString] owner:nil];
	[[NSPasteboard generalPasteboard] setString:buffer forType: NSPasteboardTypeString];
#elif defined(AQUA_PLATFORM_iOS) // Mobile
	UIPasteboard* pasteboard = [UIPasteboard generalPasteboard];
	pasteboard.string = buffer;
#endif
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