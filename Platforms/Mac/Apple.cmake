if(APPLE)
	set(AQUA_CODESIGN_TYPE "Automatic" CACHE STRING "How to sign the engine")

	# Create a dropdown in the CMake GUI using the following values
	set_property(CACHE AQUA_CODESIGN_TYPE PROPERTY STRINGS Disabled Manual Automatic)

	if(AQUA_CODESIGN_TYPE STREQUAL Manual)
		# Can be found by running `xcrun security find-identity -v -p codesigning`
		# set(AQUA_CODESIGN_SIGNATURE "" CACHE STRING "Signature when applying code signing manually")

		set(AQUA_CODESIGN_IDENTITY "" CACHE STRING "Apple Development")
		set(AQUA_CODESIGN_DEV_TEAM "" CACHE STRING "")
	endif()
endif()

function (CodeSign)
	if(NOT APPLE)
		return()
	endif()

	if(AQUA_CODESIGN_TYPE STREQUAL Disabled)
		set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "No")
	elseif(AQUA_CODESIGN_TYPE STREQUAL Manual)
		set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual")
		set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${AQUA_CODESIGN_DEV_TEAM})
		set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${AQUA_CODESIGN_IDENTITY})
	elseif(AQUA_CODESIGN_TYPE STREQUAL Automatic)
		set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic")

		if(NOT ("${AQUA_CODESIGN_DEV_TEAM}" STREQUAL ""))
			set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${AQUA_CODESIGN_DEV_TEAM})
		endif()
	endif()

	# if((AQUA_CODESIGN_TYPE STREQUAL Manual) AND NOT ("${AQUA_CODESIGN_SIGNATURE}" STREQUAL ""))
	#	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD VERBATIM
	#		# Sign executable
	#		COMMAND echo Signing executable '${PROJECT_NAME}'
	#		COMMAND codesign -s ${AQUA_CODESIGN_SIGNATURE} --timestamp $<TARGET_FILE:${PROJECT_NAME}>
	#	)
	# endif()
endfunction()

function (SetRPath)
	if(NOT APPLE)
		return()
	endif()

	get_target_property(targetType ${PROJECT_NAME} TYPE)

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		# Add @rpath to executable
		COMMAND echo Adding rpath to executable ${PROJECT_NAME}
		COMMAND install_name_tool -add_rpath @executable_path/../Resources/ $<TARGET_FILE:${PROJECT_NAME}> 	|| (exit 0) # Allow failure
		COMMAND install_name_tool -add_rpath @executable_path/../lib/ $<TARGET_FILE:${PROJECT_NAME}>		|| (exit 0) # Allow failure
	)

	if(targetType STREQUAL "EXECUTABLE")
		SET(CMAKE_INSTALL_RPATH "@executable_path/../Resources/")
	elseif(targetType STREQUAL "SHARED_LIBRARY")
		set_target_properties(${PROJECT_NAME} PROPERTIES
			BUILD_WITH_INSTALL_RPATH 1
		   INSTALL_NAME_DIR "@executable_path/../Resources/"
		)
	endif()
endfunction()

function (SetXCodeProperties)
	# Configure Mac OS XCode properties
	set_target_properties(${PROJECT_NAME} PROPERTIES
	#	XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER LFCD
	#	MACOSX_BUNDLE_GUI_IDENTIFIER LFCD
		XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER com.aqua.${PROJECT_NAME}
	)
endfunction()