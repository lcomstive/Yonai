enable_testing()
project(AquaEngineTest)

# Add tests
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} GTest::gtest_main AquaEngine ${AQUA_ENGINE_DEPENDENCY_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../Include
	${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/googletest/googletest/include
	${AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS}
)

if(APPLE)
	# Copy mono library to @executable/libs/ folder for finding in rpath
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		# Copy mono library to output
		COMMAND echo Copying mono dynamic lib to output library folder

		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/mono/native/libmonosgen-2.0.1.dylib
			${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libmonosgen-2.0.1.dylib
	)

	SetRPath()
endif()

include(GoogleTest)
gtest_discover_tests(AquaEngineTest)