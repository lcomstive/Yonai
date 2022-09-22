#pragma once

#if defined(_WIN32)
	#include <glad/gl.h>
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
		#include <glad/gles2.h>
	#elif TARGET_OS_MAC
		#include <glad/gl.h>
	#else
		#error "Unknown Apple platform"
	#endif
#elif defined(__ANDROID__)
	#include <glad/gles2.h>
#else
	// Assumed desktop platform
	#include <glad/gl.h>
#endif