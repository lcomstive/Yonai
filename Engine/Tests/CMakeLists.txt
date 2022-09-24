enable_testing()

# Get Google Test framework
find_package(GTest)

# Add tests
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(AquaEngineTest ${SOURCES})
target_link_libraries(AquaEngineTest GTest::gtest_main AquaEngine)

include(GoogleTest)
gtest_discover_tests(AquaEngineTest)