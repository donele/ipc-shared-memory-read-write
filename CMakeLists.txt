cmake_minimum_required(VERSION 3.20)
project(shmcas LANGUAGES CXX)


set(BUILD_SHARED_LIBS ON)
option(BUILD_BENCHMARKS "Build benchmark targets" ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib64")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include(CTest)

find_package(nlohmann_json REQUIRED)
add_subdirectory(src)
add_subdirectory(bin)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
