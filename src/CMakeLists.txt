# Configure Nestalgic library target
add_library(nestalgic
        cpu.cpp
)
target_include_directories(nestalgic PRIVATE ${CMAKE_SOURCE_DIR}/include)
