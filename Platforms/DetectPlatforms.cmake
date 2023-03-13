if(ANDROID OR iOS)
    set(AQUA_MOBILE_PLATFORM true)
elseif (UNIX OR APPLE OR WIN32)
    set(AQUA_DESKTOP_PLATFORM true)
else()
    message("Unable to determine if compiling for mobile or desktop platform")
endif ()

if(APPLE)
    set(AQUA_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Mac/AppIcon.icns)
    set(AQUA_EXECUTABLE_RESOURCES ${AQUA_ICON})

    set(AQUA_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Aqua\ Editor.app/Contents/Resources)
elseif(WIN32)
    set(AQUA_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Windows/AppIcon.ico)
    set(AQUA_EXECUTABLE_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Windows/AppIcon.rc)

    set(AQUA_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
    set(VS_DEBUGGER_WORKING_DIRECTORY ${AQUA_RESOURCES_DIR}/$<CONFIG>)
elseif(UNIX)
    if(BUILD_SHARED_LIBS)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()
    
	set(AQUA_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
	set(AQUA_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Mac/Apple.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Windows/Windows.cmake)

function (AddPlatformSpecifics)
    if(APPLE) ## From Mac/Apple.cmake
        # Add RPath to find resources and libraries properly
        SetRPath()

        # Sign executable
        CodeSign()
    elseif(WIN32) ## From Windows/Windows.cmake
        # Sets properties for executable
        # e.g. Show/hide console depending on debug/release build
        SetWin32Properties()
    endif()
endfunction()