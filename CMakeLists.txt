cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

# Get the engine version from Version.txt & git revision
include(${CMAKE_CURRENT_SOURCE_DIR}/Platforms/VersionInfo.cmake)

# Detect if compiling for mobile or desktop platform
include(${CMAKE_CURRENT_SOURCE_DIR}/Platforms/DetectPlatforms.cmake)

# Enable testing if AquaEngine is not a dependency
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

### ENGINE
project(AquaEngine)
add_subdirectory(Engine)

### APPS
add_subdirectory(Apps)

### ORGANISATION
include(Platforms/Folders.cmake)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AquaEditorLauncher)
