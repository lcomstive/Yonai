enable_testing()

# Get Google Test framework
find_package(GTest)

# Add tests
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(AquaEngineTest ${SOURCES})
target_link_libraries(AquaEngineTest GTest::gtest_main AquaEngine ${AQUA_ENGINE_DEPENDENCY_LIBS})
target_include_directories(AquaEngineTest PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../Include
	${AQUA_ENGINE_DEPENDENCY_INCLUDE_DIRS}
)

include(GoogleTest)
gtest_discover_tests(AquaEngineTest)