cmake_minimum_required(VERSION 3.12)

project(nestalgic VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

cmake_minimum_required(VERSION 3.12)
project(nestalgic VERSION 0.1.0
        DESCRIPTION "A simple NES emulator"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
# Add the -g flag for debug symbols and disable optimizations
set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_subdirectory(src)
add_subdirectory(test)
