#pragma once

// Platform identification
#if defined(_WIN32)
	#define YONAI_PLATFORM_WINDOWS
	#define YONAI_PLATFORM_NAME "Windows"
#elif defined(__APPLE__)
	#define YONAI_PLATFORM_APPLE

    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        // iOS, tvOS, or watchOS device
		#define YONAI_PLATFORM_iOS
		#define YONAI_PLATFORM_NAME "iOS"
    #elif TARGET_OS_MAC
        // Other kinds of Apple platforms
		#define YONAI_PLATFORM_MAC
		#define YONAI_PLATFORM_NAME "Mac OS"
    #else
    #   error "Unknown Apple platform"
    #endif
#elif defined(__ANDROID__)
	#define YONAI_PLATFORM_ANDROID
	#define YONAI_PLATFORM_NAME "Android"
#elif __linux__
	#define YONAI_PLATFORM_LINUX
	#define YONAI_PLATFORM_NAME "Linux"
#elif defined(__unix__)
	#define YONAI_PLATFORM_UNIX
	#define YONAI_PLATFORM_NAME "Unix"
#else
	#error "Unknown platform"
#endif

#if defined(YONAI_PLATFORM_WINDOWS) || \
	defined(YONAI_PLATFORM_MAC) || \
	defined(YONAI_PLATFORM_LINUX)
	#define YONAI_PLATFORM_DESKTOP
#endif

#if defined(YONAI_PLATFORM_iOS) || \
	defined(YONAI_PLATFORM_ANDROID)
	#define YONAI_PLATFORM_MOBILE
#endif

#if BUILD_SHARED_LIBS
	#if defined(_MSC_VER)
		#define YONAI_EXPORT __declspec(dllexport)
		#define YONAI_IMPORT __declspec(dllimport)
	#elif defined(__GNUC__)
		#define YONAI_EXPORT __attribute__((visibility("default")))
		#define YONAI_IMPORT
	#else
		#define YONAI_EXPORT
		#define YONAI_IMPORT
	#endif

	#if defined(YONAI_ENGINE_EXPORT_DLL)
		#define YonaiAPI YONAI_EXPORT
	#else
		#define YonaiAPI YONAI_IMPORT
	#endif
#else
	#define YONAI_EXPORT
	#define YONAI_IMPORT
	#define YonaiAPI
#endif

#ifndef YONAI_VERSION_MAJOR
#define YONAI_VERSION_MAJOR 0
#endif
#ifndef YONAI_VERSION_MINOR
#define YONAI_VERSION_MINOR 0
#endif
#ifndef YONAI_VERSION_PATCH
#define YONAI_VERSION_PATCH 0
#endif
#ifndef YONAI_VERSION_REV
#define YONAI_VERSION_REV 0
#endif
#ifndef YONAI_VERSION_BRANCH
#define YONAI_VERSION_BRANCH ""
#endif

#ifndef YONAI_VERSION_STRING
#include <string>
#define YONAI_VERSION_STRING \
	std::string("v" + \
			std::to_string(YONAI_VERSION_MAJOR) + "." + \
			std::to_string(YONAI_VERSION_MINOR) + "." + \
			std::to_string(YONAI_VERSION_PATCH)		+ \
			"-" YONAI_VERSION_REV						  \
			" [" YONAI_VERSION_BRANCH "]" )
#endif

// Silence errors
#pragma warning(disable : 4251)