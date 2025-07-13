cmake_minimum_required(VERSION 3.10)
project(sffs VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(TARGET utest)
  message("utest has been added")
else()
  add_subdirectory(third_party/utest)
endif()

if(TARGET benchmark::benchmark)
  message("benchmark has been added")
else()
  add_subdirectory(third_party/benchmark)
endif()

add_library(sffs INTERFACE ${sffs_SOURCES})
target_include_directories(sffs INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_subdirectory(test)
add_subdirectory(benchmark)
