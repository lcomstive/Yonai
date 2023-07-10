project(BaseGame)
file(GLOB_RECURSE SOURCE_FILES Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES Include/*.hpp)

add_executable(${PROJECT_NAME}
	${SOURCE_FILES}
	${HEADER_FILES}
	${COMMON_SOURCE_FILES}
	${COMMON_HEADER_FILES}
	${EXECUTABLE_RESOURCES}
)

# Group source files inside IDE
source_group("Resources" FILES ${EXECUTABLE_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source"  PREFIX "Source"  FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include" PREFIX "Include" FILES ${INCLUDE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
	Include
	${CMAKE_SOURCE_DIR}/Engine/Include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Yonai
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	_CRT_SECURE_NO_WARNINGS
	__STDC_FORMAT_MACROS
)

add_dependencies(${PROJECT_NAME} YonaiScriptCore)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	# Copy standard assets to output directory
	COMMAND echo Copying standard assets to output directory
	
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/../Editor/Assets/
		${YONAI_RESOURCES_DIR}/Assets/
)

AddPlatformSpecifics()