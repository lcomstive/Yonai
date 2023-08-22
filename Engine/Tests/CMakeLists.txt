enable_testing()
project(YonaiTest)

# Add tests
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} GTest::gtest_main Yonai ${YONAI_DEPENDENCY_LIBS} imgui)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../Include
	${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/googletest/googletest/include
	${YONAI_DEPENDENCY_INCLUDE_DIRS}
)

if(APPLE)
	if(BUILD_SHARED_LIBS)
		# Copy mono library to @executable/libs/ folder for finding in rpath
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			# Copy mono library to output
			COMMAND echo Copying mono dynamic lib to output library folder

			COMMAND ${CMAKE_COMMAND} -E copy
				${MONO_SHARED_LIB}
				${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
		)
	endif()

	SetRPath()
elseif(WIN32)
	# /ignore:4099 - Ignores warning when no pdb (debug info) is found with linking target (such as the mono library)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

include(GoogleTest)
gtest_discover_tests(YonaiTest)
