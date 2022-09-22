#pragma once

// Platform identification
#if defined(_WIN32)
	#define AQUA_PLATFORM_WINDOWS
	#define AQUA_PLATFORM_NAME "Windows"
#elif defined(__APPLE__)
	#define AQUA_PLATFORM_APPLE

    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        // iOS, tvOS, or watchOS device
		#define AQUA_PLATFORM_iOS
		#define AQUA_PLATFORM_NAME "iOS"
    #elif TARGET_OS_MAC
        // Other kinds of Apple platforms
		#define AQUA_PLATFORM_MAC
		#define AQUA_PLATFORM_NAME "Mac OS"
    #else
    #   error "Unknown Apple platform"
    #endif
#elif defined(__ANDROID__)
	#define AQUA_PLATFORM_ANDROID
	#define AQUA_PLATFORM_NAME "Android"
#elif __linux__
	#define AQUA_PLATFORM_LINUX
	#define AQUA_PLATFORM_NAME "Linux"
#elif defined(__unix__)
	#define AQUA_PLATFORM_UNIX
	#define AQUA_PLATFORM_NAME "Unix"
#else
	#error "Unknown platform"
#endif

#if defined(AQUA_PLATFORM_WINDOWS) || \
	defined(AQUA_PLATFORM_MAC) || \
	defined(AQUA_PLATFORM_LINUX)
	#define AQUA_PLATFORM_DESKTOP
#endif

#if defined(AQUA_PLATFORM_iOS) || \
	defined(AQUA_PLATFORM_ANDROID)
	#define AQUA_PLATFORM_MOBILE
#endif

#if BUILD_SHARED_LIBS
	#if defined(_MSC_VER)
		#define AQUA_EXPORT __declspec(dllexport)
		#define AQUA_IMPORT __declspec(dllimport)
	#elif defined(__GNUC__)
		#define AQUA_EXPORT __attribute__((visibility("default")))
		#define AQUA_IMPORT
	#else
		#define AQUA_EXPORT
		#define AQUA_IMPORT
	#endif

	#if defined(AQUA_ENGINE_EXPORT_DLL)
		#define AquaAPI AQUA_EXPORT
	#else
		#define AquaAPI AQUA_IMPORT
	#endif
#else
	#define AQUA_EXPORT
	#define AQUA_IMPORT
	#define AquaAPI
#endif

#ifndef AQUA_ENGINE_VERSION_MAJOR
#define AQUA_ENGINE_VERSION_MAJOR 0
#endif
#ifndef AQUA_ENGINE_VERSION_MINOR
#define AQUA_ENGINE_VERSION_MINOR 0
#endif
#ifndef AQUA_ENGINE_VERSION_PATCH
#define AQUA_ENGINE_VERSION_PATCH 0
#endif
#ifndef AQUA_ENGINE_VERSION_REV
#define AQUA_ENGINE_VERSION_REV 0
#endif
#ifndef AQUA_ENGINE_VERSION_BRANCH
#define AQUA_ENGINE_VERSION_BRANCH ""
#endif

#ifndef AQUA_ENGINE_VERSION_STRING
#include <string>
#define AQUA_ENGINE_VERSION_STRING \
	std::string("v" + \
			std::to_string(AQUA_ENGINE_VERSION_MAJOR) + "." + \
			std::to_string(AQUA_ENGINE_VERSION_MINOR) + "." + \
			std::to_string(AQUA_ENGINE_VERSION_PATCH)		+ \
			"-" AQUA_ENGINE_VERSION_REV						  \
			" [" AQUA_ENGINE_VERSION_BRANCH "]" )
#endif

// Silence errors
#pragma warning(disable : 4251)