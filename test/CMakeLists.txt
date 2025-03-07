enable_testing()

# Setup testing dependencies
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Configure test target
add_executable(nestalgic_tests
        cpu_test.cpp
        cpu_bus_test.cpp
)

target_include_directories(nestalgic_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(nestalgic_tests PRIVATE nestalgic GTest::gtest GTest::gmock GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(nestalgic_tests)
