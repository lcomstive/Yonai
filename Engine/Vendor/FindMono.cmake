#[[
    Tries to find Mono installation on system in the following paths:

    Windows
        - C:/Program Files/Mono
        - %MONO_ROOT%

    Mac OS
        - /opt/homebrew
        - /Library/Frameworks/Mono.framework

    Linux
        - /usr
        - /usr/local
       
    Defines:

        MONO_FOUND          True if mono installation found
        MONO_ROOT           Mono root directory
        MONO_LIBRARY        Library to link
        MONO_LIBRARY_DIRS   Directory containing MONO_LIBRARIES
        MONO_INCLUDE_DIRS   Directory containing header files

    To override Mono install directory, add a CMake variable MONO_ROOT

]]

set(MONO_FOUND       FALSE)
set(MONO_LIBRARY_DIRS "")
set(MONO_INCLUDE_DIRS "")

function(CheckMonoInstallDir rootDir)
    if((NOT EXISTS ${rootDir}) OR (NOT EXISTS ${rootDir}/include/mono-2.0))
        return()
    endif()

    set(MONO_ROOT ${rootDir} PARENT_SCOPE)
    set(MONO_LIBRARY_DIRS ${rootDir}/lib PARENT_SCOPE)
    set(MONO_INCLUDE_DIRS ${rootDir}/include/mono-2.0 PARENT_SCOPE)
    set(MONO_FOUND TRUE PARENT_SCOPE)

    if(BUILD_SHARED_LIBS)
        find_file(MONO_LIBRARY NAMES
            libmono-static-sgen.lib # Windows only runs with static linkage
            libmonosgen-2.0.so      # Linux
            libmonosgen-2.0.dylib   # Mac
            PATHS "${rootDir}/lib" REQUIRED)
    else()
        find_file(MONO_LIBRARY NAMES
            libmono-static-sgen.lib # Windows
            libmonosgen-2.0.a       # Linux / Mac
            PATHS "${rootDir}/lib" REQUIRED)
    endif()

    message("Mono library: ${MONO_LIBRARY}")

    if(WIN32)
        set(MONO_LIBRARY ${MONO_LIBRARY} ws2_32 winmm version bcrypt PARENT_SCOPE)
    elseif(APPLE)
        set(MONO_LIBRARY ${MONO_LIBRARY} iconv PARENT_SCOPE)
    endif()
endfunction()

set(MONO_POTENTIAL_DIRS ${MONO_ROOT})
if(NOT("$ENV{MONO_ROOT}" STREQUAL ""))
    list(APPEND MONO_POTENTIAL_DIRS "$ENV{MONO_ROOT}")
endif()

if(WIN32)
    list(APPEND MONO_POTENTIAL_DIRS "C:\\Program Files\\Mono")
elseif(APPLE)
    list(APPEND MONO_POTENTIAL_DIRS "/opt/homebrew")
    list(APPEND MONO_POTENTIAL_DIRS "/Library/Frameworks/Mono.framework/Versions/Current")
elseif(UNIX)
    list(APPEND MONO_POTENTIAL_DIRS "/usr")
    list(APPEND MONO_POTENTIAL_DIRS "/usr/local")
endif()

foreach(monoDir IN LISTS MONO_POTENTIAL_DIRS)
    CheckMonoInstallDir(${monoDir})
    if(MONO_FOUND)
        break()
    endif()
endforeach()
