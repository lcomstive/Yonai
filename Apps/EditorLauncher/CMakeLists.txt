project(AquaEditorLauncher)
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp")
file(GLOB_RECURSE HEADER_FILES "Include/*.hpp")

add_executable(
	${PROJECT_NAME}
	${SOURCE_FILES}
	${HEADER_FILES}
	${EXECUTABLE_RESOURCES}
)

add_dependencies(${PROJECT_NAME} AquaEditor)
target_link_libraries(${PROJECT_NAME} PRIVATE AquaEngine)
target_include_directories(${PROJECT_NAME} PUBLIC
	Include
	${CMAKE_SOURCE_DIR}/Engine/Include
)

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		LINK_FLAGS_DEBUG   			"/SUBSYSTEM:CONSOLE"
		LINK_FLAGS_RELEASE 			"/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
		LINK_FLAGS_MINSIZEREL 		"/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
		LINK_FLAGS_RELWITHDEBINFO 	"/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
	)
endif()

# Group source files inside IDE
source_group("Resources" FILES ${EXECUTABLE_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source"  PREFIX "Source" FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" PREFIX "Include" FILES ${INCLUDE_FILES})


if(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/AquaEditor.app/Contents/MacOS/
	)
endif()