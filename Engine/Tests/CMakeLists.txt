enable_testing()

# Get Google Test framework
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add tests
add_executable(AquaEngineTest ECS.cpp)
target_link_libraries(AquaEngineTest GTest::gtest_main AquaEngine)

include(GoogleTest)
gtest_discover_tests(AquaEngineTest)