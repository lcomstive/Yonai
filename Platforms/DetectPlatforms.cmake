if(ANDROID OR iOS)
    set(YONAI_MOBILE_PLATFORM true)
elseif (UNIX OR APPLE OR WIN32)
    set(YONAI_DESKTOP_PLATFORM true)
else()
    message("Unable to determine if compiling for mobile or desktop platform")
endif ()

if(APPLE)
    set(YONAI_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Mac/AppIcon.icns)
    set(YONAI_EXECUTABLE_RESOURCES ${YONAI_ICON})

    set(YONAI_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Yonai\ Editor.app/Contents/Resources)

    if(CMAKE_GENERATOR STREQUAL Xcode)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/bin)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/bin)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/lib)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/lib)
    endif()

elseif(WIN32)
    set(YONAI_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Windows/AppIcon.ico)
    set(YONAI_EXECUTABLE_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Windows/AppIcon.rc)

    set(YONAI_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
    set(VS_DEBUGGER_WORKING_DIRECTORY ${YONAI_RESOURCES_DIR}/$<CONFIG>)
elseif(UNIX)
    if(BUILD_SHARED_LIBS)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()
    
    set(YONAI_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
    set(YONAI_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Mac/Apple.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Unix/Unix.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Windows/Windows.cmake)

function (AddPlatformSpecifics)
    if(APPLE) ## From Mac/Apple.cmake
        # Add RPath to find resources and libraries properly
        SetRPath()

        # Sign executable
        CodeSign()

        # Set XCode properties
        SetXCodeProperties()
    elseif(WIN32) ## From Windows/Windows.cmake
        # Sets properties for executable
        # e.g. Show/hide console depending on debug/release build
        SetWin32Properties()

        if(MSVC)
            # Enable multi-processor compilation for faster builds
            target_compile_options(${PROJECT_NAME} PRIVATE "/MP")

            target_compile_definitions(${PROJECT_NAME} PUBLIC _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
        endif()
    endif()
endfunction()
