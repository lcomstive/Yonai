set(YONAI_DEFINES)
set(YONAI_DEPENDENCY_LIBS)
set(YONAI_DEPENDENCY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)

# Unix requires pthread
if(UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	list(APPEND YONAI_DEPENDENCY_LIBS Threads::Threads pthread)
endif()

# Linux requires some specific libraries for compilation
if(UNIX AND NOT APPLE)
	list(APPEND YONAI_DEPENDENCY_LIBS
		dl	# dynamic library interface
		rt	
		z	# zlib 
		X11 # X11 graphics lib
	)
endif()

# Vulkan
if(YONAI_GRAPHICS_VULKAN)
	find_package(Vulkan REQUIRED)
	message("Vulkan version v${Vulkan_VERSION}")
	list(APPEND YONAI_DEPENDENCY_INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS})
	list(APPEND YONAI_DEPENDENCY_LIBS Vulkan::Vulkan)
	list(APPEND YONAI_DEFINES YONAI_GRAPHICS_VULKAN)
endif()

# GLFW
if(YONAI_DESKTOP_PLATFORM)
	set(GLFW_INSTALL OFF CACHE BOOL "")

	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glfw)
	include_directories(${GLFW_INCLUDE_DIRS})
	list(APPEND YONAI_DEPENDENCY_LIBS glfw)
endif()

# GLM
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm)
list(APPEND YONAI_DEPENDENCY_LIBS glm)

# SPDLog
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/spdlog)
list(APPEND YONAI_DEPENDENCY_LIBS spdlog)

# STB
list(APPEND YONAI_DEPENDENCY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/stb)

# Assimp
set(ASSIMP_INSTALL OFF CACHE BOOL "")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
if(NOT APPLE)
	set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "")
endif()

# Disable exporting
set(ASSIMP_NO_EXPORT ON CACHE BOOL "")

# Set importers supported by default
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "")
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "")
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/assimp EXCLUDE_FROM_ALL)
list(APPEND YONAI_DEPENDENCY_LIBS assimp)

# ImGui and ImGuizmo
if(YONAI_BUILD_EDITOR)
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

		# ImGuizmo
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/GraphEditor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/GraphEditor.h
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImCurveEdit.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImCurveEdit.h
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImGradient.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImGradient.h
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImGuizmo.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImGuizmo.h
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImSequencer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImSequencer.h
		${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo/ImZoomSlider.h
	)
	target_include_directories(imgui PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGuizmo>
		$<INSTALL_INTERFACE:include>)
endif() # YONAI_BUILD_EDITOR

# X11 on Linux
if(LINUX)
    list(APPEND YONAI_DEPENDENCY_LIBS X11)
endif()

# Mono
include(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/FindMono.cmake)
if(MONO_FOUND)
	link_directories(${MONO_LIBRARY_DIRS})
	list(APPEND YONAI_DEPENDENCY_LIBS ${MONO_LIBRARY})
	list(APPEND YONAI_DEPENDENCY_INCLUDE_DIRS ${MONO_INCLUDE_DIRS})

	set(MONO_ROOT "${MONO_ROOT}" PARENT_SCOPE)
	set(MONO_LIBRARY "${MONO_LIBRARY}" PARENT_SCOPE)
	set(MONO_SHARED_LIB "${MONO_SHARED_LIB}" PARENT_SCOPE)
	set(MONO_LIBRARY_DIRS "${MONO_LIBRARY_DIRS}" PARENT_SCOPE)

    if(WIN32)
        set(MONO_MDB2PDB ${MONO_ROOT}/bin/pdb2mdb.bat PARENT_SCOPE)
	else()
		set(MONO_MDB2PDB ${MONO_ROOT}/bin/pdb2mdb PARENT_SCOPE)
    endif()
endif()

list(APPEND YONAI_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/mono/include")

# Portable File Dialogs
list(APPEND YONAI_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/portable-file-dialogs")

# Google Test framework
if(YONAI_BUILD_TESTS)
	set(INSTALL_GTEST OFF CACHE BOOL "")
	set(BUILD_GMOCK OFF CACHE BOOL "")
	set(gtest_force_shared_crt ON CACHE BOOL "")
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/googletest EXCLUDE_FROM_ALL)
endif()

# MiniAudio
list(APPEND YONAI_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/miniaudio")

# Apple Frameworks
if(APPLE)
	list(APPEND YONAI_DEPENDENCY_LIBS "-framework AudioUnit -framework Foundation")

	if(NOT YONAI_MOBILE_PLATFORM)
		list(APPEND YONAI_DEPENDENCY_LIBS "-framework AppKit")
	elseif(YONAI_MOBILE_PLATFORM)
		list(APPEND YONAI_DEPENDENCY_LIBS "-framework UIKit")
	endif()
endif()

foreach(DEFINE IN LISTS YONAI_DEFINES)
	add_definitions(-D${DEFINE})
endforeach()
