project(AquaEditor)
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp")
file(GLOB_RECURSE HEADER_FILES "Include/*.hpp")

add_executable(
	${PROJECT_NAME}
	${SOURCE_FILES}
	${HEADER_FILES}
	${AQUA_EXECUTABLE_RESOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
	Include
	${CMAKE_SOURCE_DIR}/Engine/Include
)

target_link_libraries(${PROJECT_NAME} PRIVATE AquaEngine)

# Add dependency to the C# binding project
add_dependencies(${PROJECT_NAME} AquaScriptCore)

if(WIN32 OR APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Aqua Editor")
endif()

if(APPLE)
	# Configure Mac OS app bundle
	set_target_properties(
		${PROJECT_NAME} PROPERTIES
		BUNDLE TRUE
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_GUI_IDENTIFIER com.madissia.${PROJECT_NAME}
		MACOSX_BUNDLE_BUNDLE_NAME "Aqua Editor"
		MACOSX_BUNDLE_COPYRIGHT "© 2023 Madissia Technologies"
		MACOSX_BUNDLE_EXECUTABLE_NAME "AquaEditorLauncher"
		MACOSX_BUNDLE_ICON_FILE ${AQUA_ICON}
		MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING_LONG}
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING}
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../EditorLauncher/Info.plist.in
		XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.madissia.AquaEditor"
	)

	set_source_files_properties(${AQUA_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

# Group source files inside IDE
source_group("Resources" FILES ${EXECUTABLE_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source"  PREFIX "Source" FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" PREFIX "Include" FILES ${INCLUDE_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	# Copy standard assets to output directory
	COMMAND echo Copying standard assets to output directory
	
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Assets/
		${AQUA_RESOURCES_DIR}/Assets/
)

if(APPLE)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		# Copy mono library to output
		COMMAND echo Copying mono lib to output

		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_SOURCE_DIR}/../../Engine/Vendor/mono/native/libmonosgen-2.0.1.dylib
			${AQUA_RESOURCES_DIR}
		)

	if(BUILD_SHARED_LIBS)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			# Copy AquaEngine library to output
			COMMAND echo Copying AquaEngine dynamic lib to output

			COMMAND ${CMAKE_COMMAND} -E copy
				$<TARGET_FILE:AquaEditor>
				${AQUA_RESOURCES_DIR}
		)
	endif()
endif()

AddPlatformSpecifics()