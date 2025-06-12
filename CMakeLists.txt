cmake_minimum_required(VERSION 3.16...3.29)
project(AutoProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    message(FATAL_ERROR "This project is intended for Clang/LLVM.")
else()
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_RC_COMPILER llvm-rc)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

file(GLOB SRC "src/*.cpp")
include_directories(include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
    set(RC_FILE "${CMAKE_SOURCE_DIR}/resources/icon.rc")
    set(RES_FILE "${CMAKE_BINARY_DIR}/icon.res")

    add_custom_command(
        OUTPUT "${RES_FILE}"
        COMMAND ${CMAKE_RC_COMPILER} /FO "${RES_FILE}" "${RC_FILE}"
        DEPENDS "${RC_FILE}" "${CMAKE_SOURCE_DIR}/resources/auto.ico"
        COMMENT "Compiling icon resource file"
        VERBATIM
    )


    add_executable(auto ${SRC} ${RES_FILE})
    target_link_libraries(auto PRIVATE Shlwapi)
else()
    add_executable(auto ${SRC})
endif()

target_compile_options(auto PRIVATE -Wall -Wextra -Wpedantic)
