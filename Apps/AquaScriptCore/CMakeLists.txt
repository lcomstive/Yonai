if(MSVC)
    # Visual Studio
    project(AquaScriptCore LANGUAGES CSharp)

    file(GLOB_RECURSE CS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cs)

    add_library(${PROJECT_NAME} SHARED ${CS_FILES})

    set_target_properties(${PROJECT_NAME} PROPERTIES
        DEBUG_POSTFIX ""
        VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
        VS_DOTNET_REFERENCES "${CMAKE_CURRENT_SOURCE_DIR}/../Editor/Assets/Mono/Newtonsoft.Json.dll;System"
    )

    # Copy AquaScriptCore.dll to output resources mono directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "Copying AquaScriptCore.dll to app://Assets/Mono/"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${AQUA_RESOURCES_DIR}/Assets/Mono/AquaScriptCore.dll
    )

    if(AQUA_GENERATE_MDBS)
        # Generate MDB
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND $<IF:$<CONFIG:Release>,echo,${MONO_MDB2PDB}> $<TARGET_FILE:${PROJECT_NAME}>)
        # Copy MDB to resources mono directory 
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND $<IF:$<CONFIG:Release>,echo,${CMAKE_COMMAND}> -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/AquaScriptCore.dll.mdb ${AQUA_RESOURCES_DIR}/Assets/Mono/AquaScriptCore.dll.mdb)
    endif()

else()
    # Generator is not Visual Studio.
    # AquaScriptCore will have to be compiled manually,
    # this target will copy the expected output to the binary directory
    add_custom_target(
        AquaScriptCore
        COMMENT "Copying AquaScriptCore.dll to app://Assets/Mono/"
        COMMAND mkdir -p ${AQUA_RESOURCES_DIR}
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/AquaScriptCore.* ${AQUA_RESOURCES_DIR}/Assets/Mono/
    )
endif()