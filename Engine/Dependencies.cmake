cmake_policy(SET CMP0072 NEW) # Prefer newer OpenGL libraries over legacy ones

set(AQUA_ENGINE_DEPENDENCY_LIBS)
set(AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "")

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
if(AQUA_DESKTOP_PLATFORM)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")

    add_subdirectory(Vendor/glfw)
    list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS glfw)
    list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glfw/include")
endif()

# GLM
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm")

# Cereal
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/cereal/include")

# SPDLog
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/spdlog/include")

# STB
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/stb")

# Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(Vendor/assimp)
list(APPEND AQUA_ENGINE_DEPENDENCY_LIBS assimp)
list(APPEND AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/assimp/include")

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
