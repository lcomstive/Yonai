project(YonaiEditor)
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp")
file(GLOB_RECURSE HEADER_FILES "Include/YonaiEditor/*.hpp" "Include/YonaiEditor/*.h")

list(APPEND HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/YonaiEditor/Glue.hpp")

add_executable(
	${PROJECT_NAME}
	${SOURCE_FILES}
	${HEADER_FILES}
	${YONAI_EXECUTABLE_RESOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
	Include
	${CMAKE_SOURCE_DIR}/Engine/Include
)

target_link_libraries(${PROJECT_NAME} PRIVATE Yonai imgui)

# Add dependency to the C# binding projects
add_dependencies(${PROJECT_NAME} YonaiScriptCore YonaiScriptEditor BaseGame)

# Generate C# bindings from C#
add_custom_command(
	OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Include/YonaiEditor/Glue.hpp"
	COMMAND GlueGenerator "${CMAKE_CURRENT_SOURCE_DIR}/Source" "${CMAKE_CURRENT_SOURCE_DIR}/Include/YonaiEditor/Glue.hpp"
	DEPENDS GlueGenerator
)

if(WIN32 OR APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Yonai Editor")
endif()

if(APPLE)
	# Configure Mac OS app bundle
	set_target_properties(
		${PROJECT_NAME} PROPERTIES
		BUNDLE TRUE
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_GUI_IDENTIFIER com.madissia.${PROJECT_NAME}
		MACOSX_BUNDLE_BUNDLE_NAME "Yonai Editor"
		MACOSX_BUNDLE_COPYRIGHT "© 2023 Madissia Technologies"
		MACOSX_BUNDLE_EXECUTABLE_NAME "YonaiEditor"
		MACOSX_BUNDLE_ICON_FILE ${YONAI_ICON}
		MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING_LONG}
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING}
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
	)

	set_source_files_properties(${YONAI_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

# Group source files inside IDE
source_group("Resources" FILES ${EXECUTABLE_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source"  PREFIX "Source" FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" PREFIX "Include" FILES ${INCLUDE_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	# Copy standard assets to output directory
	COMMAND echo Copying standard assets to output directory
	
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Assets
		${YONAI_RESOURCES_DIR}/Assets

	# Copy .NET .dlls for this platform to the root of the Assets/Mono/
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${YONAI_RESOURCES_DIR}/Assets/Mono/$<IF:$<PLATFORM_ID:Windows>,Windows,Unix>
		${YONAI_RESOURCES_DIR}/Assets/Mono

	# Remove platform .NET .dll folders
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${YONAI_RESOURCES_DIR}/Assets/Mono/Windows
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${YONAI_RESOURCES_DIR}/Assets/Mono/Unix
)

if(BUILD_SHARED_LIBS AND MONO_SHARED_LIB)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD	
		# Copy mono shared library to output
		COMMAND echo Copying Mono dynamic lib to output

		COMMAND ${CMAKE_COMMAND} -E copy
			${MONO_SHARED_LIB}
			${YONAI_RESOURCES_DIR}
	)
endif()

if(APPLE)
	if(BUILD_SHARED_LIBS)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			# Copy YonaiEngine library to output
			COMMAND echo Copying YonaiEngine dynamic lib to output

			COMMAND ${CMAKE_COMMAND} -E copy
				$<TARGET_FILE:Yonai>
				${YONAI_RESOURCES_DIR}

			COMMAND echo Copying dependency dynamic libs to output
			COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.dylib
				${YONAI_RESOURCES_DIR}
		)
	endif()

	find_file(MONO_LIBRARY_NATIVE NAMES
		libmono-native.dylib   			# Homebrew
		libmono-native-compat.dylib   	# Default Framework
		PATHS "${MONO_LIBRARY_DIRS}" REQUIRED)

	find_file(MONO_LIBRARY_POSIXHELPER NAMES libMonoPosixHelper.dylib PATHS "${MONO_LIBRARY_DIRS}" REQUIRED)

	# Copy base game to resource directory
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND echo Copying base game to output
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BaseGame ${YONAI_RESOURCES_DIR}/BaseGame
	)	

	# Copy required Mono .dylibs to resource directory
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND echo Copying ${MONO_LIBRARY_NATIVE} to output
		COMMAND ${CMAKE_COMMAND} -E copy ${MONO_LIBRARY_NATIVE} ${YONAI_RESOURCES_DIR}/libmono-native.dylib
		COMMAND ${CMAKE_COMMAND} -E copy ${MONO_LIBRARY_POSIXHELPER} ${YONAI_RESOURCES_DIR}
	)
endif()

AddPlatformSpecifics()

include(${CMAKE_CURRENT_SOURCE_DIR}/Install.cmake)