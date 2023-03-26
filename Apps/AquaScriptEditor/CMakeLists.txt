if(MSVC)
    # Visual Studio
    project(AquaScriptEditor LANGUAGES CSharp)

    file(GLOB_RECURSE CS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cs)

    add_library(${PROJECT_NAME} SHARED ${CS_FILES})
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")

    add_dependencies(${PROJECT_NAME} AquaScriptCore)

    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "")
else()
    # Generator is not Visual Studio.
    # AquaScriptEditor will have to be compiled manually,
    # this target will copy the expected output to the binary directory
    add_custom_target(
        AquaScriptEditor
        COMMENT "Copying AquaScriptEditor to output directory"
        COMMAND mkdir -p ${AQUA_RESOURCES_DIR}
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/AquaScriptEditor.* ${AQUA_RESOURCES_DIR}
    )
endif()