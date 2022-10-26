cmake_policy(SET CMP0072 NEW) # Prefer newer OpenGL libraries over legacy ones

set(AQUA_ENGINE_DEPENDENCY_LIBS)
set(AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "")

# Unix requires pthread library
if(UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS Threads::Threads pthread)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
if(AQUA_DESKTOP_PLATFORM)
	find_package(glfw3)
	include_directories(${GLFW_INCLUDE_DIRS})
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glfw)
endif()

# GLM
find_package(glm)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glm::glm)

# SPDLog
find_package(spdlog)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS spdlog::spdlog)

# STB
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glm::glm)

# Assimp
find_package(assimp)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS assimp::assimp)

# Glad
if(AQUA_DESKTOP_PLATFORM)
    add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glad/src/gl.c)
elseif(ANDROID)
    add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glad/src/gles2.c)
endif()

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glad/include)

list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glad)
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glad/include")

# ImGUI
find_package(imgui)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS imgui::imgui)

# X11 on Linux
if(LINUX)
    list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS X11)
endif()

# Mono
if(WIN32)
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS
		ws2_32.lib
		version.lib
		Bcrypt.lib
		winmm.lib
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/mono/native/libmono-static-sgen.lib
	)
elseif(APPLE)
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/mono/native/libmonosgen-2.0.1.dylib)
elseif(UNIX)
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS
		libz.so
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/mono/native/libmonosgen-2.0.a
	)
endif()

list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/mono/include")
