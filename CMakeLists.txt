cmake_minimum_required(VERSION 3.0.0)

project(xSamples)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE -O3)
endif ()

include(cmake/implot.cmake)

include(cmake/config.cmake)
include(cmake/samples.cmake)

add_compile_definitions(XENGINE_EXPORT=)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug Build")
else ()
    message("Release Build")
    add_compile_definitions("ENABLE_APPLICATION_EXCEPTION_HANDLER")
endif ()