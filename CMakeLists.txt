cmake_minimum_required(VERSION 3.12)

project(purenes VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

cmake_minimum_required(VERSION 3.12)
project(purenes VERSION 0.1.0
        DESCRIPTION "A simple NES emulator"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

# Configure PureNES library target
add_library(purenes STATIC
        src/cpu.cpp
        )

target_include_directories(purenes PUBLIC include/purenes)
set_target_properties(purenes PROPERTIES VERSION ${PROJECT_VERSION})


# Setup testing dependencies
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Configure test target
add_executable(purenes_tests
        test/cpu/cpu_test.cpp
        )

target_include_directories(purenes_tests PRIVATE include/purenes)
target_link_libraries(purenes_tests gtest_main)

include(GoogleTest)

gtest_discover_tests(purenes_tests)
