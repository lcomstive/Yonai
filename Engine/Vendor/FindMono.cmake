#[[
    Tries to find Mono installation on system in the following paths:

    Windows
        - C:/Program Files/Mono/
        - %MONO_ROOT%

    Mac OS
        - /Library/Frameworks/Mono.framework/

    Linux
        - /usr/
        - /usr/local/
       
    Defines:

        MONO_FOUND          True if mono installation found
        MONO_ROOT           Mono root directory
        MONO_LIBRARY        Library to link
        MONO_LIBRARY_DIRS   Directory containing MONO_LIBRARIES
        MONO_INCLUDE_DIRS   Directory containing header files
        MONO_SHARED_LIB     Shared library to copy to binary directory. Empty if BUILD_SHARED_LIBS is off.

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

    #[[
    if(BUILD_SHARED_LIBS)
        find_library(MONO_LIBRARY NAMES mono-2.0-sgen PATHS "${rootDir}/lib" REQUIRED)

        if(WIN32)
            set(MONO_LIBRARY ${MONO_LIBRARY} ws2_32 winmm version bcrypt PARENT_SCOPE)
            set(MONO_SHARED_LIB "${rootDir}/bin/mono-2.0-sgen.dll" PARENT_SCOPE)            
        else()
            set(MONO_SHARED_LIB "${MONO_LIBRARY}" PARENT_SCOPE)
        endif()
    else()
    ]]
        find_library(MONO_LIBRARY NAMES
            libmono-static-sgen
            mono-static-sgen
            libmonosgen-2.0
            monosgen-2.0
            PATHS "${rootDir}/lib" REQUIRED)
    # endif()

    if(WIN32)
        set(MONO_LIBRARY ${MONO_LIBRARY} ws2_32 winmm version bcrypt PARENT_SCOPE)
    endif()
endfunction()

set(MONO_POTENTIAL_DIRS ${MONO_ROOT} "$ENV{MONO_ROOT}")

if(WIN32)
    list(APPEND MONO_POTENTIAL_DIRS "C:\\Program Files\\Mono")
elseif(APPLE)
    list(APPEND MONO_POTENTIAL_DIRS "/Library/Frameworks/Mono.framework")
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
