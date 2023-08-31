if(MSVC)
	# Visual Studio
	project(YonaiScriptEditor LANGUAGES CSharp)

	file(GLOB_RECURSE CS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cs)

	add_library(${PROJECT_NAME} SHARED ${CS_FILES})

	add_dependencies(${PROJECT_NAME} YonaiScriptCore)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		DEBUG_POSTFIX ""
		VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
		VS_DOTNET_REFERENCES "${CMAKE_CURRENT_SOURCE_DIR}/../Editor/Assets/Mono/Newtonsoft.Json.dll;System"
	)

	# Copy YonaiScriptEditor.dll to output resources mono directory
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMENT "Copying YonaiScriptEditor.dll to app://Assets/Mono/"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${YONAI_RESOURCES_DIR}/Assets/Mono/YonaiScriptEditor.dll
	)

	if(YONAI_GENERATE_MDBS)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			# Generate MDB
            COMMAND $<IF:$<CONFIG:Release,MinSizeRel>,echo,${MONO_MDB2PDB}> "$<TARGET_FILE:${PROJECT_NAME}>"
        	# Copy MDB to resources mono directory 
            COMMAND $<IF:$<CONFIG:Release,MinSizeRel>,echo,${CMAKE_COMMAND}> -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/YonaiScriptEditor.dll.mdb ${YONAI_RESOURCES_DIR}/Assets/Mono/YonaiScriptEditor.dll.mdb)
    endif()
else()
	# Generator is not Visual Studio.
	# YonaiScriptEditor will have to be compiled manually,
	# this target will copy the expected output to the binary directory
	add_custom_target(
		YonaiScriptEditor
		COMMENT "Copying YonaiScriptEditor to output directory"
		COMMAND mkdir -p ${YONAI_RESOURCES_DIR}/Assets/Mono
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/YonaiScriptEditor.* ${YONAI_RESOURCES_DIR}/Assets/Mono/
	)
endif()