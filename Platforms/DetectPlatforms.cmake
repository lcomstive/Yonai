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

    if(MSVC)
        set(AQUA_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
        set(VS_DEBUGGER_WORKING_DIRECTORY ${AQUA_RESOURCES_DIR})
    else()
    	set(AQUA_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()
else()
	set(AQUA_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
