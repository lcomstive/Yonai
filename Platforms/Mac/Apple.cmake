if(APPLE)
	set(AQUA_CODESIGN_SIGNATURE "" CACHE STRING "Signature when applying code signing")
endif()

function (CodeSign)
	if(NOT APPLE)
		return()
	endif()

	if(NOT ("${AQUA_CODESIGN_SIGNATURE}" STREQUAL ""))
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD VERBATIM
			# Sign executable
			COMMAND echo Signing executable '${PROJECT_NAME}'
			COMMAND codesign -s ${AQUA_CODESIGN_SIGNATURE} --timestamp $<TARGET_FILE:${PROJECT_NAME}>
		)
	endif()
endfunction()

function (SetRPath)
	get_target_property(targetType ${PROJECT_NAME} TYPE)
	if(NOT APPLE OR NOT (targetType STREQUAL "EXECUTABLE"))
		return()
	endif()

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		# Add @rpath to executable
		COMMAND echo Adding rpath to executable ${PROJECT_NAME}
		COMMAND install_name_tool -add_rpath @executable_path/../Resources/ $<TARGET_FILE:${PROJECT_NAME}>
		COMMAND install_name_tool -add_rpath @executable_path/../lib/ $<TARGET_FILE:${PROJECT_NAME}>
	)
endfunction()