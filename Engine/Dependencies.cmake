cmake_policy(SET CMP0072 NEW) # Prefer newer OpenGL libraries over legacy ones

set(AQUA_ENGINE_DEPENDENCY_LIBS)
set(AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "")

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
include_directories(${GLM_INCLUDE_DIRS})
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glm::glm)

# SPDLog
find_package(spdlog)
include_directories(${SPDLOG_INCLUDE_DIRS})
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS spdlog::spdlog)

# STB
include_directories(${STB_INCLUDE_DIRS})
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glm::glm)

# Assimp
find_package(assimp)
include_directories(${ASSIMP_INCLUDE_DIRS})
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

# X11 on Linux
if(LINUX)
    list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS X11)
endif()
