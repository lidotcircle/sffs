cmake_minimum_required(VERSION 3.10)
project(sffs VERSION 0.1.0)

include("cmake/CPM.cmake")
include("cmake/googletest.cmake")
include("cmake/GoogleBenchmark.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")


add_library(sffs INTERFACE ${sffs_SOURCES})
target_include_directories(sffs INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

OPTION(CXX_VERSION "C++ version" OFF)
if(CXX_VERSION STREQUAL OFF)
    SET(CXX_VERSION 17)
endif()

add_subdirectory(test)
add_subdirectory(benchmark)
