if(MSVC)
	# Visual Studio
	project(AquaScriptEditor LANGUAGES CSharp)

	file(GLOB_RECURSE CS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cs)

	add_library(${PROJECT_NAME} SHARED ${CS_FILES})

	add_dependencies(${PROJECT_NAME} AquaScriptCore)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		DEBUG_POSTFIX ""
		VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
		VS_DOTNET_REFERENCES "${CMAKE_CURRENT_SOURCE_DIR}/../Editor/Assets/Mono/Newtonsoft.Json.dll;System"
	)

	# Copy AquaScriptEditor.dll to output resources mono directory
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMENT "Copying AquaScriptEditor.dll to app://Assets/Mono/"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${AQUA_RESOURCES_DIR}/Assets/Mono/AquaScriptEditor.dll
	)

	if(AQUA_GENERATE_MDBS)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND $<IF:$<CONFIG:Release>,echo,${MONO_MDB2PDB}> $<TARGET_FILE:${PROJECT_NAME}>)
		# Copy MDB to resources mono directory 
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND $<IF:$<CONFIG:Release>,echo,${CMAKE_COMMAND}> -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/AquaScriptEditor.dll.mdb ${AQUA_RESOURCES_DIR}/Assets/Mono/AquaScriptEditor.dll.mdb)
	endif()
else()
	# Generator is not Visual Studio.
	# AquaScriptEditor will have to be compiled manually,
	# this target will copy the expected output to the binary directory
	add_custom_target(
		AquaScriptEditor
		COMMENT "Copying AquaScriptEditor to output directory"
		COMMAND mkdir -p ${AQUA_RESOURCES_DIR}/Assets/Mono
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/AquaScriptEditor.* ${AQUA_RESOURCES_DIR}/Assets/Mono/
	)
endif()