cmake_policy(SET CMP0072 NEW) # Prefer newer OpenGL libraries over legacy ones

set(AQUA_ENGINE_DEPENDENCY_LIBS)
set(AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)

# Unix requires pthread
if(UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS Threads::Threads pthread)
endif()

# Linux requires some specific libraries for compilation
if(UNIX AND NOT APPLE)
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS
		dl	# dynamic library interface
		rt	
		z	# zlib 
		X11 # X11 graphics lib
	)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
if(AQUA_DESKTOP_PLATFORM)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glfw)
	include_directories(${GLFW_INCLUDE_DIRS})
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glfw)
endif()

# GLM
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glm)

# SPDLog
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/spdlog)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS spdlog)

# STB
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/stb)

# RapidJSON
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/rapidjson/include)

# Assimp
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/assimp)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS assimp)

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
project(imgui)

add_library(imgui STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui.h
	${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imstb_rectpack.h
	${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imstb_textedit.h
	${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imstb_truetype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui>
    $<INSTALL_INTERFACE:include>)

list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS imgui)

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
	list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/mono/native/libmonosgen-2.0.so.1)
endif()

list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/mono/include")

# Portable File Dialogs
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/portable-file-dialogs")

# Google Test framework
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/googletest)