cmake_minimum_required(VERSION 3.16)
project(AutoProject LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use Clang for compilation
if(MSVC)
    message(FATAL_ERROR "This project is intended for Clang/MinGW")
else()
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_RC_COMPILER windres) # Resource compiler
endif()

# Source files
file(GLOB SRC "src/*.cpp")
file(GLOB SRC_TEST "test/*.cpp")
set(RESOURCES resources/resource.res)

# Include directories
include_directories(include)

# Output binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Main executable
add_executable(auto ${SRC} ${RESOURCES})
target_compile_options(auto PRIVATE -Wall -Wextra -Wpedantic)

# Icon compilation (Windows)
if(WIN32)
    add_custom_command(
        OUTPUT ${RESOURCES}
        COMMAND ${CMAKE_RC_COMPILER} resources/icon.rc -O coff -o ${RESOURCES}
        DEPENDS resources/icon.rc
    )
endif()

# Experimental test executable
add_executable(test_experimental ${SRC_TEST})
target_compile_options(test_experimental PRIVATE -Wall -Wextra -Wpedantic)

# Clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E rm -rf CMakeFiles CMakeCache.txt
)
